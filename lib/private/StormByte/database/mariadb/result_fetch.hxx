#pragma once

#include <StormByte/database/rows.hxx>
#include <mysql.h>
#include <iostream>

namespace StormByte::Database::MariaDB {
    inline ExpectedRows StepResults(MYSQL_RES* res) noexcept {
        if (!res)
            return Unexpected<QueryException>(ExecuteError("Invalid MYSQL_RES provided."));

        Rows rows;
        int nrows = static_cast<int>(mysql_num_rows(res));
        int nfields = mysql_num_fields(res);
        for (int r = 0; r < nrows; ++r) {
            MYSQL_ROW row = mysql_fetch_row(res);
            unsigned long* lengths = mysql_fetch_lengths(res);
            Row prow;
            for (int c = 0; c < nfields; ++c) {
                MYSQL_FIELD* field = mysql_fetch_field_direct(res, c);
                const char* colName = field ? field->name : nullptr;
                
                if (!row[c]) {
                    prow.add(std::string(colName ? colName : ""), Value());
                    continue;
                }
                unsigned long len = lengths ? lengths[c] : 0;
                enum_field_types ftype = field ? field->type : MYSQL_TYPE_STRING;
                switch (ftype) {
                    case MYSQL_TYPE_TINY:
                    case MYSQL_TYPE_SHORT:
                    case MYSQL_TYPE_LONG: {
                        long long v = 0;
                        try { v = std::stoll(std::string(row[c])); } catch(...) { v = 0; }
                        if (v > std::numeric_limits<int>::max() || v < std::numeric_limits<int>::min())
                            prow.add(std::string(colName ? colName : ""), static_cast<long int>(v));
                        else
                            prow.add(std::string(colName ? colName : ""), static_cast<int>(v));
                        break;
                    }
                    case MYSQL_TYPE_LONGLONG: {
                        long long v = 0;
                        try { v = std::stoll(std::string(row[c])); } catch(...) { v = 0; }
                        prow.add(std::string(colName ? colName : ""), static_cast<long int>(v));
                        break;
                    }
                    case MYSQL_TYPE_FLOAT:
                    case MYSQL_TYPE_DOUBLE: {
                        double d = 0.0;
                        try { d = std::stod(std::string(row[c])); } catch(...) { d = 0.0; }
                        prow.add(std::string(colName ? colName : ""), d);
                        break;
                    }
                    case MYSQL_TYPE_BLOB:
                    case MYSQL_TYPE_VAR_STRING:
                    case MYSQL_TYPE_STRING:
                    default: {
                            // treat as blob only for known binary columns (e.g. 'data'), otherwise as text
                            if (ftype == MYSQL_TYPE_BLOB && field && field->name && std::string(field->name) == "data") {
                                std::vector<std::byte> blob;
                                if (len > 0) blob.assign(reinterpret_cast<const std::byte*>(row[c]), reinterpret_cast<const std::byte*>(row[c]) + len);
                                prow.add(std::string(colName ? colName : ""), std::move(blob));
                            } else {
                                prow.add(std::string(colName ? colName : ""), std::string(row[c] ? row[c] : ""));
                            }
                            break;
                        }
                }
            }
            rows.add(std::move(prow));
        }

        return rows;
    }
}
