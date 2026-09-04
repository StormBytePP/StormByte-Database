/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * StormByte-Database is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * or later, as published by the Free Software Foundation.
 *
 * StormByte-Database is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte-Database. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

#include <StormByte/database/mariadb/prepared_stmt.hxx>
#include <StormByte/database/mariadb/result_fetch.hxx>
#include <cstdint>
#include <cstring>
#include <limits>
#include <mysql.h>
#include <string>
#include <vector>
using namespace StormByte::Database::MariaDB;
static inline MYSQL* to_mysql_conn(struct st_mysql* c) noexcept {
	return reinterpret_cast<MYSQL*>(c);
}
static inline MYSQL_STMT* to_mysql_stmt(struct st_mysql_stmt* s) noexcept {
	return reinterpret_cast<MYSQL_STMT*>(s);
}
static inline struct st_mysql_stmt* to_st_mysql_stmt(MYSQL_STMT* s) noexcept {
	return reinterpret_cast<struct st_mysql_stmt*>(s);
}
void PreparedSTMT::EnsureParamSize(std::vector<StormByte::Database::Value>& params, int index) noexcept {
	if (index < 0) return;
	if (static_cast<size_t>(index) >= params.size()) params.resize(index + 1);
}
PreparedSTMT::PreparedSTMT(const std::string& name, const std::string& query, struct st_mysql* conn, std::shared_ptr<Logger::Log> logger)
	: Database::PreparedSTMT(name, query, std::move(logger)), m_conn(conn), m_stmt(nullptr) {
	MYSQL* cpp_conn = to_mysql_conn(m_conn);
	MYSQL_STMT* stmt = mysql_stmt_init(cpp_conn);
	if (!stmt) {
		if (m_logger) {
			*m_logger << Logger::Level::Error << "MariaDB: mysql_stmt_init returned null for query: " << Query() << std::endl;
		}
		return;
	}
	std::string prepq = Query();
	while (!prepq.empty() && isspace(static_cast<unsigned char>(prepq.back()))) prepq.pop_back();
	if (!prepq.empty() && prepq.back() == ';') prepq.pop_back();
	if (mysql_stmt_prepare(stmt, prepq.c_str(), static_cast<unsigned long>(prepq.size())) != 0) {
		if (m_logger) {
			*m_logger << Logger::Level::Error << "MariaDB: mysql_stmt_prepare error: "
					<< (mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "<none>")
					<< " for query: " << Query() << std::endl;
		}
		mysql_stmt_close(stmt);
		return;
	}
	m_stmt = to_st_mysql_stmt(stmt);
}
PreparedSTMT::PreparedSTMT(std::string&& name, std::string&& query, struct st_mysql* conn, std::shared_ptr<Logger::Log> logger) noexcept
	: Database::PreparedSTMT(std::move(name), std::move(query), std::move(logger)), m_conn(conn), m_stmt(nullptr) {
	MYSQL* cpp_conn = to_mysql_conn(m_conn);
	MYSQL_STMT* stmt = mysql_stmt_init(cpp_conn);
	if (!stmt) return;
	std::string prepq = Query();
	while (!prepq.empty() && isspace(static_cast<unsigned char>(prepq.back()))) prepq.pop_back();
	if (!prepq.empty() && prepq.back() == ';') prepq.pop_back();
	if (mysql_stmt_prepare(stmt, prepq.c_str(), static_cast<unsigned long>(prepq.size())) != 0) {
		mysql_stmt_close(stmt);
		return;
	}
	m_stmt = to_st_mysql_stmt(stmt);
}
PreparedSTMT::~PreparedSTMT() noexcept {
	if (m_stmt) {
		MYSQL_STMT* stmt = to_mysql_stmt(m_stmt);
		mysql_stmt_close(stmt);
		m_stmt = nullptr;
	}
}
void PreparedSTMT::Binder(const int& index, Value&& value) noexcept {
	EnsureParamSize(m_params, index);
	m_params[index] = std::move(value);
}
void PreparedSTMT::Reset() noexcept {
	m_params.clear();
	if (m_stmt) {
		mysql_stmt_reset(to_mysql_stmt(m_stmt));
	}
}
StormByte::Database::ExpectedRows PreparedSTMT::DoExecute() {
	if (!m_conn || !m_stmt) {
		return Unexpected<ExecuteError>("No DB connection or statement");
	}
	MYSQL_STMT* stmt = to_mysql_stmt(m_stmt);
	std::vector<MYSQL_BIND> bind_in;
	bind_in.resize(m_params.size());
	std::vector<int32_t> int_buf(m_params.size());
	std::vector<uint32_t> uint_buf(m_params.size());
	std::vector<int64_t> ll_buf(m_params.size());
	std::vector<uint64_t> ull_buf(m_params.size());
	std::vector<double> dbl_buf(m_params.size());
	std::vector<char> bool_buf(m_params.size());
	std::vector<std::string> str_buf(m_params.size());
	std::vector<std::vector<char>> bin_buf(m_params.size());
	std::vector<unsigned long> str_len(m_params.size());
	std::vector<my_bool> is_null(m_params.size());
	for (size_t i = 0; i < m_params.size(); ++i) {
		memset(&bind_in[i], 0, sizeof(MYSQL_BIND));
		const StormByte::Database::Value& p = m_params[i];
		is_null[i] = 0;
		if (p.IsNull()) {
			bind_in[i].buffer_type = MYSQL_TYPE_NULL;
			bind_in[i].is_null = &is_null[i];
			is_null[i] = 1;
			continue;
		}
		auto vt = p.Type();
		switch (vt) {
			case StormByte::Database::Value::Type::Integer: {
				int_buf[i] = p.Get<int>();
				bind_in[i].buffer_type = MYSQL_TYPE_LONG;
				bind_in[i].buffer = &int_buf[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::UnsignedInteger: {
				uint_buf[i] = p.Get<unsigned int>();
				bind_in[i].buffer_type = MYSQL_TYPE_LONG;
				bind_in[i].buffer = &uint_buf[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::LongInteger: {
				ll_buf[i] = p.Get<long int>();
				bind_in[i].buffer_type = MYSQL_TYPE_LONGLONG;
				bind_in[i].buffer = &ll_buf[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::UnsignedLongInteger: {
				ull_buf[i] = p.Get<unsigned long int>();
				bind_in[i].buffer_type = MYSQL_TYPE_LONGLONG;
				bind_in[i].buffer = &ull_buf[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::Double: {
				dbl_buf[i] = p.Get<double>();
				bind_in[i].buffer_type = MYSQL_TYPE_DOUBLE;
				bind_in[i].buffer = &dbl_buf[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::Boolean: {
				bool_buf[i] = p.Get<bool>() ? 1 : 0;
				bind_in[i].buffer_type = MYSQL_TYPE_TINY;
				bind_in[i].buffer = &bool_buf[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::Text: {
				str_buf[i] = p.Get<std::string>();
				bind_in[i].buffer_type = MYSQL_TYPE_STRING;
				bind_in[i].buffer = const_cast<char*>(str_buf[i].data());
				bind_in[i].buffer_length = static_cast<unsigned long>(str_buf[i].size());
				str_len[i] = bind_in[i].buffer_length;
				bind_in[i].length = &str_len[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::Blob: {
				auto bv = p.Get<std::vector<std::byte>>();
				bin_buf[i].resize(bv.size());
				for (size_t k = 0; k < bv.size(); ++k)
					bin_buf[i][k] = static_cast<char>(bv[k]);
				bind_in[i].buffer_type = MYSQL_TYPE_BLOB;
				bind_in[i].buffer = bin_buf[i].data();
				bind_in[i].buffer_length = static_cast<unsigned long>(bin_buf[i].size());
				str_len[i] = bind_in[i].buffer_length;
				bind_in[i].length = &str_len[i];
				bind_in[i].is_null = &is_null[i];
				break;
			}
			case StormByte::Database::Value::Type::Null:
			default: {
				bind_in[i].buffer_type = MYSQL_TYPE_NULL;
				bind_in[i].is_null = &is_null[i];
				is_null[i] = 1;
				break;
			}
		}
	}
	if (!bind_in.empty()) {
		if (mysql_stmt_bind_param(stmt, bind_in.data()) != 0) {
			return Unexpected<ExecuteError>(mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "Unknown MySQL stmt error");
		}
	}
	if (mysql_stmt_execute(stmt) != 0) {
		return Unexpected<ExecuteError>(mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "Unknown MySQL stmt error");
	}
	MYSQL_RES* meta = mysql_stmt_result_metadata(stmt);
	if (!meta) {
		if (mysql_stmt_field_count(stmt) == 0) {
			return Rows();
		}
		return Unexpected<ExecuteError>(mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "Unknown MySQL stmt error");
	}
	const unsigned int nfields = mysql_num_fields(meta);
	std::vector<MYSQL_BIND> bind_out(nfields);
	std::vector<unsigned long> out_len(nfields);
	std::vector<my_bool> out_is_null(nfields);
	std::vector<std::vector<char>> out_str(nfields);
	std::vector<int32_t> out_int(nfields);
	std::vector<uint32_t> out_uint(nfields);
	std::vector<int64_t> out_ll(nfields);
	std::vector<uint64_t> out_ull(nfields);
	std::vector<double> out_dbl(nfields);
	std::vector<char> out_bool(nfields);
	for (unsigned int i = 0; i < nfields; ++i) {
		memset(&bind_out[i], 0, sizeof(MYSQL_BIND));
		MYSQL_FIELD* f = mysql_fetch_field_direct(meta, i);
		unsigned long blen = (f && f->length) ? f->length : 1024;
		out_is_null[i] = 0;
		switch (f ? f->type : MYSQL_TYPE_STRING) {
			case MYSQL_TYPE_TINY:
				if (f && (f->flags & UNSIGNED_FLAG) == 0 && f->length == 1) {
					bind_out[i].buffer_type = MYSQL_TYPE_TINY;
					bind_out[i].buffer = &out_bool[i];
				} else {
					bind_out[i].buffer_type = MYSQL_TYPE_LONG;
					if (f && (f->flags & UNSIGNED_FLAG)) {
						bind_out[i].buffer = &out_uint[i];
					} else {
						bind_out[i].buffer = &out_int[i];
					}
				}
				bind_out[i].is_null = &out_is_null[i];
				bind_out[i].length = &out_len[i];
				break;
			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_LONG:
				bind_out[i].buffer_type = MYSQL_TYPE_LONG;
				if (f && (f->flags & UNSIGNED_FLAG)) {
					bind_out[i].buffer = &out_uint[i];
				} else {
					bind_out[i].buffer = &out_int[i];
				}
				bind_out[i].is_null = &out_is_null[i];
				bind_out[i].length = &out_len[i];
				break;
			case MYSQL_TYPE_LONGLONG:
				bind_out[i].buffer_type = MYSQL_TYPE_LONGLONG;
				if (f && (f->flags & UNSIGNED_FLAG)) {
					bind_out[i].buffer = &out_ull[i];
				} else {
					bind_out[i].buffer = &out_ll[i];
				}
				bind_out[i].is_null = &out_is_null[i];
				bind_out[i].length = &out_len[i];
				break;
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_DOUBLE:
				bind_out[i].buffer_type = MYSQL_TYPE_DOUBLE;
				bind_out[i].buffer = &out_dbl[i];
				bind_out[i].is_null = &out_is_null[i];
				bind_out[i].length = &out_len[i];
				break;
			case MYSQL_TYPE_BLOB:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
			default:
				out_str[i].resize(blen + 1);
				bind_out[i].buffer_type = MYSQL_TYPE_STRING;
				bind_out[i].buffer = out_str[i].data();
				bind_out[i].buffer_length = static_cast<unsigned long>(out_str[i].size());
				bind_out[i].length = &out_len[i];
				bind_out[i].is_null = &out_is_null[i];
				break;
		}
	}
	if (mysql_stmt_bind_result(stmt, bind_out.data()) != 0) {
		mysql_free_result(meta);
		return Unexpected<ExecuteError>(mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "Unknown MySQL stmt error");
	}
	if (mysql_stmt_store_result(stmt) != 0) {
		mysql_free_result(meta);
		return Unexpected<ExecuteError>(mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "Unknown MySQL stmt error");
	}
	Rows rows;
	while (true) {
		int rc = mysql_stmt_fetch(stmt);
		if (rc == MYSQL_NO_DATA) break;
		if (rc != 0 && rc != MYSQL_DATA_TRUNCATED) {
			mysql_free_result(meta);
			return Unexpected<ExecuteError>(mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "Unknown MySQL stmt fetch error");
		}
		if (rc == MYSQL_DATA_TRUNCATED) {
			for (unsigned int ci = 0; ci < nfields; ++ci) {
				if (out_is_null[ci]) continue;
				if (out_len[ci] > bind_out[ci].buffer_length) {
					out_str[ci].resize(out_len[ci] + 1);
					bind_out[ci].buffer = out_str[ci].data();
					bind_out[ci].buffer_length = static_cast<unsigned long>(out_str[ci].size());
					if (mysql_stmt_fetch_column(stmt, &bind_out[ci], ci, 0) != 0) {
						mysql_free_result(meta);
						return Unexpected<ExecuteError>(mysql_stmt_error(stmt) ? mysql_stmt_error(stmt) : "Unknown MySQL stmt fetch_column error");
					}
				}
			}
		}
		Row prow;
		for (unsigned int i = 0; i < nfields; ++i) {
			MYSQL_FIELD* f = mysql_fetch_field_direct(meta, i);
			const char* colName = f ? f->name : nullptr;
			if (out_is_null[i]) {
				prow.add(std::string(colName ? colName : ""), Value());
				continue;
			}
			switch (f ? f->type : MYSQL_TYPE_STRING) {
				case MYSQL_TYPE_TINY: {
					if (f && (f->flags & UNSIGNED_FLAG) == 0 && f->length == 1) {
						prow.add(std::string(colName ? colName : ""), static_cast<bool>(out_bool[i] != 0));
					} else if (f && (f->flags & UNSIGNED_FLAG)) {
						prow.add(std::string(colName ? colName : ""), static_cast<unsigned int>(out_uint[i]));
					} else {
						prow.add(std::string(colName ? colName : ""), static_cast<int>(out_int[i]));
					}
					break;
				}
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_LONG:
					if (f && (f->flags & UNSIGNED_FLAG))
						prow.add(std::string(colName ? colName : ""), static_cast<unsigned int>(out_uint[i]));
					else
						prow.add(std::string(colName ? colName : ""), static_cast<int>(out_int[i]));
					break;
				case MYSQL_TYPE_LONGLONG:
					if (f && (f->flags & UNSIGNED_FLAG))
						prow.add(std::string(colName ? colName : ""), static_cast<unsigned long int>(out_ull[i]));
					else
						prow.add(std::string(colName ? colName : ""), static_cast<long int>(out_ll[i]));
					break;
				case MYSQL_TYPE_FLOAT:
				case MYSQL_TYPE_DOUBLE:
					prow.add(std::string(colName ? colName : ""), out_dbl[i]);
					break;
				case MYSQL_TYPE_BLOB: {
					unsigned long llen = out_len[i];
					// 63 = binary charset; otherwise treat as text (TEXT/VARCHAR)
					const bool is_binary = f && f->charsetnr == 63;
					if (is_binary) {
						std::vector<std::byte> blob;
						if (llen > 0) {
							blob.resize(llen);
							for (unsigned long bi = 0; bi < llen; ++bi)
								blob[bi] = static_cast<std::byte>(out_str[i][bi]);
						}
						prow.add(std::string(colName ? colName : ""), std::move(blob));
					} else {
						std::string sval(out_str[i].data(), llen);
						prow.add(std::string(colName ? colName : ""), std::move(sval));
					}
					break;
				}
				case MYSQL_TYPE_VAR_STRING:
				case MYSQL_TYPE_STRING:
				default: {
					unsigned long llen = out_len[i];
					std::string sval(out_str[i].data(), llen);
					prow.add(std::string(colName ? colName : ""), std::move(sval));
					break;
				}
			}
		}
		rows.add(std::move(prow));
	}
	mysql_free_result(meta);
	mysql_stmt_free_result(stmt);
	return rows;
}
