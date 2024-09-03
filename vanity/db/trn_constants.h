//
// Created by kingsli on 9/3/24.
//

#ifndef VANITY_TRN_CONSTANTS_H
#define VANITY_TRN_CONSTANTS_H

#include <cstdint>

namespace vanity {

using trn_id_t = uint64_t;

// the trn_id for db operations that aren't part of a transaction
constexpr trn_id_t NO_TRN_ID = 0;

// the trn_id for internal expiry operations
constexpr trn_id_t EXPIRY_TRN_ID = 1;

// reserved trn_ids
constexpr trn_id_t RESERVED_TRN_ID_1 = 2;

constexpr trn_id_t RESERVED_TRN_ID_2 = 3;

// the first trn_id that can be used by clients
constexpr trn_id_t FIRST_CLIENT_TRN_ID = 4;

} // namespace vanity

#endif //VANITY_TRN_CONSTANTS_H
