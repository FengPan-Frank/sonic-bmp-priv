/*
 * Copyright (c) 2024 Microsoft, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#include "RedisManager.h"


/*********************************************************************//**
 * Constructor for class
 ***********************************************************************/
RedisManager() : redis_(nullptr), connected_(false) {
    neiTableEnable_ = true;
    ribInTableEnable_ = true;
    ribOutTableEnable_ = true;
}

/*********************************************************************//**
 * Get singleton instance for class
 ***********************************************************************/
static RedisManager& RedisManager::getInstance(); {
    static RedisManager instance;
    return instance;
}

/*********************************************************************
 * Setup logger for this class
 *
 * \param [in] logPtr     logger pointer
 ***********************************************************************/
void RedisManager::Setup(Logger *logPtr) {
    logger_ = logPtr;
}


/**
 * Reset all Tables once FRR reconnects to BMP, this will not disable table population
 *
 * \param [in] N/A
 */
bool RedisManager::ResetAllTables() {

}


/**
 * Connect to redis and maintain the context
 *
 * \param [in] N/A
 */
bool RedisManager::ConnectRedis() {
    if (connected_)
        return true;

    SELF_INFO("RedisManager Connect Redis host = 127.0.0.1, port = 6379");

    redisContext* redis = redisConnect("127.0.0.1", 6379);
    if (redis == NULL || redis->err) {
        sELF_INFO("RedisManager failed to connect to Redis %s", redis->errstr);
            return false;
    }

    redis_ = redis;
    connected_ = true; 
    return true;
}

/**
 * WriteBGPNeighborTable
 *
 * \param [in] neighbor  Reference to neighbor address
 * \param [in] field     Reference to the specific field name
 * \param [in] value     Reference to the value to be set
 */
bool RedisManager::WriteBGPNeighborTable(const std::string& neighbor, const std::string& field, const std::string& value) {
    if (!neiTableEnable_) {
        SELF_INFO("RedisManager BGPNeighborTable is disabled");
        return false;
    }

    if (!connected_) {
        if (!ConnectRedis()) {
            return false;
        }
    }

    string key = BMP_TABLE_NEI + ":" + neighbor;
    SELF_DEBUG("RedisManager WriteBGPNeighborTable neighbor = %s: field = %s: value = %s", neighbor.c_str(), field.c_str(), value.c_str());
        
    redisReply* reply = (redisReply*)redisCommand(redis_, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (reply == NULL) {
        SELF_INFO("RedisManager WriteBGPNeighborTable executing HSET command failed: %s", redis_->errstr);
        return false;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        SELF_INFO("RedisManager WriteBGPNeighborTable HSET command reply: %s", reply->str);
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    return true;
}

/**
 * WriteBGPInRibTable
 *
 * \param [in] neighbor  Reference to neighbor address
 * \param [in] nlri      Reference to nlri
 * \param [in] field     Reference to the specific field name
 * \param [in] value     Reference to the value to be set
 */
bool RedisManager::WriteBGPRibInTable(const std::string& neighbor, const std::string& nlri, const std::string& field, const std::string& value) {
    if (!ribInTableEnable_) {
        SELF_INFO("RedisManager BGPRibInTable is disabled");
        return false;
    }
    if (!connected_) {
        if (!ConnectRedis()) {
            return false;
        }
    }

    string key = BMP_TABLE_RIB_IN + ":" + nlri + BMP_TABLE_NEI_PREFIX + ":" + neighbor;
    SELF_DEBUG("RedisManager WriteBGPRibInTable neighbor = %s: field = %s: value = %s", neighbor.c_str(), field.c_str(), value.c_str());

    redisReply* reply = (redisReply*)redisCommand(redis_, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (reply == NULL) {
        SELF_INFO("RedisManager WriteBGPRibInTable executing HSET command failed: %s", redis_->errstr);
        return false;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        SELF_INFO("RedisManager WriteBGPRibInTable HSET command reply: %s", reply->str);
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    return true;
}


/**
 * WriteBGPOutRibTable
 *
 * \param [in] neighbor  Reference to neighbor address
 * \param [in] nlri      Reference to nlri
 * \param [in] field     Reference to the specific field name
 * \param [in] value     Reference to the value to be set
 */
bool RedisManager::WriteBGPRibOutTable(const std::string& neighbor, const std::string& nlri, const std::string& field, const std::string& value) {
    if (!ribOutTableEnable_) {
        SELF_INFO("RedisManager BGPRibOutTable is disabled");
        return false;
    }
    if (!connected_) {
        if (!ConnectRedis()) {
            return false;
        }
    }

    string key = BMP_TABLE_RIB_OUT + ":" + nlri + BMP_TABLE_NEI_PREFIX + ":" + neighbor;
    SELF_DEBUG("RedisManager WriteBGPRibOutTable neighbor = %s: field = %s: value = %s", neighbor.c_str(), field.c_str(), value.c_str());

    redisReply* reply = (redisReply*)redisCommand(redis_, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (reply == NULL) {
        SELF_INFO("RedisManager WriteBGPRibOutTable executing HSET command failed: %s", redis_->errstr);
        return false;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        SELF_INFO("RedisManager WriteBGPRibOutTable HSET command reply: %s", reply->str);
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    return true;
}

/**
 * Enable BGP_Neighbor* Table, this will work with reset
 *
 * \param [in] N/A
 */
bool RedisManager::EnableBGPNeighborTable() {
    neiTableEnable_ = true;
    return true;
}

/**
 * Disable BGP_Neighbor* Table, this will work with reset
 *
 * \param [in] N/A
 */
bool RedisManager::DisableBGPNeighborTable() {
    neiTableEnable_ = false;
    return ResetBGPNeighborTable();
}


/**
 * Reset BGP_Neighbor* Table, this will flush redis
 *
 * \param [in] N/A
 */
bool RedisManager::ResetBGPNeighborTable() {
    SELF_INFO("RedisManager ResetBGPNeighborTable");
    if (!connected_) {
        if (!ConnectRedis()) {
            return false;
        }
    }

    redisReply* reply = static_cast<redisReply*>(redisCommand(redis_, BMP_TABLE_NEI_KEYS));
    if (reply == NULL) {
        SELF_INFO("RedisManager ResetBGPNeighborTable executing HSET command failed: %s", redis_->errstr);
        return false;
    }

    for (size_t i = 0; i < reply->elements; i++) {
        std::string key = reply->element[i]->str;
        redisReply* delReply = static_cast<redisReply*>(redisCommand(redis_, "DEL %s", key.c_str()));
        if (delReply == NULL) {
            SELF_INFO("RedisManager ResetBGPNeighborTable executing HSET command: %s", redis_->errstr);
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(delReply);
    }
    freeReplyObject(reply);
    return true;
}

/**
 * Enable BGP_RIB_IN* Table
 *
 * \param [in] N/A
 */
bool RedisManager::EnableBGPRibInTable() {
    ribInTableEnable_ = true;
    return true;
}

/**
 * Disable BGP_Rib_In* Table, this will work with reset
 *
 * \param [in] N/A
 */
bool RedisManager::DisableBGPRibInTable() {
    neiTableEnable_ = false;
    return ResetBGPRibInTable();
}

/**
    * Reset BGP_RIB_IN* Table
    *
    * \param [in] N/A
    */
bool RedisManager::ResetBGPRibInTable() {
    SELF_INFO("RedisManager ResetBGPRibInTable");
    if (!connected_) {
        if (!ConnectRedis()) {
            return false;
        }
    }

    redisReply* reply = static_cast<redisReply*>(redisCommand(redis_, BMP_TABLE_RIB_IN_KEYS));
    if (reply == NULL) {
        SELF_INFO("RedisManager ResetBGPRibInTable executing HSET command failed: %s", redis_->errstr);
        return false;
    }

    for (size_t i = 0; i < reply->elements; i++) {
        std::string key = reply->element[i]->str;
        redisReply* delReply = static_cast<redisReply*>(redisCommand(redis_, "DEL %s", key.c_str()));
        if (delReply == NULL) {
            SELF_INFO("RedisManager ResetBGPRibInTable executing HSET command: %s", redis_->errstr);
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(delReply);
    }
    freeReplyObject(reply);
    return true;
}


/**
 * Enable BGP_RIB_OUT* Table
 *
 * \param [in] N/A
 */
bool RedisManager::EnableBGPRibOutTable() {
    ribOutTableEnable_ = true;
    return true;
}

/**
 * Disable BGP_Rib_Out* Table, this will work with reset
 *
 * \param [in] N/A
 */
bool RedisManager::DisableBGPRibOutTable() {
    ribOutTableEnable_ = false;
    return ResetBGPRibOutTable();
}

/**
    * Reset BGP_RIB_OUT* Table
    *
    * \param [in] N/A
    */
bool RedisManager::ResetBGPRibOutTable() {
    SELF_INFO("RedisManager ResetBGPRibOutTable");
    if (!connected_) {
        if (!ConnectRedis()) {
            return false;
        }
    }

    redisReply* reply = static_cast<redisReply*>(redisCommand(redis_, BMP_TABLE_RIB_OUT_KEYS));
    if (reply == NULL) {
        SELF_INFO("RedisManager ResetBGPRibOutTable executing HSET command failed: %s", redis_->errstr);
        return false;
    }

    for (size_t i = 0; i < reply->elements; i++) {
        std::string key = reply->element[i]->str;
        redisReply* delReply = static_cast<redisReply*>(redisCommand(redis_, "DEL %s", key.c_str()));
        if (delReply == NULL) {
            SELF_INFO("RedisManager ResetBGPRibOutTable executing HSET command: %s", redis_->errstr);
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(delReply);
    }
    freeReplyObject(reply);
    return true;
}

/**
 * Reset all Tables once FRR reconnects to BMP, this will not disable table population
 *
 * \param [in] N/A
 */
bool RedisManager::ResetAllTables() {
    return ResetBGPNeighborTable() && ResetBGPRibInTable() && ResetBGPRibOutTable();
}