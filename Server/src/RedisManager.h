/*
 * Copyright (c) 2024 Microsoft, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#ifndef REDISMANAGER_H_
#define REDISMANAGER_H_

#include <sonic-swss-common/common/dbconnector.h>
#include <sonic-swss-common/common/table.h>
#include <sonic-swss-common/common/select.h>
#include <sonic-swss-common/common/selectableevent.h>
#include <sonic-swss-common/common/subscriberstatetable.h>

#include <string>
#include <list>
#include <map>
#include <unordered_set>
#include <functional>
#include <vector>
#include "Logger.h"
#include "BMPListener.h"


/**
 * BMP_TABLE_* defines the default table name prefix
 */
#define BMP_DB_NAME                "BMP_STATE_DB"
#define BMP_TABLE_NEI              "BGP_NEIGHBOR_TABLE"
#define BMP_TABLE_RIB_IN           "BGP_RIB_IN_TABLE"
#define BMP_TABLE_RIB_OUT          "BGP_RIB_OUT_TABLE"
#define BMP_TABLE_NEI_PREFIX       "BGP_NEIGHBOR"


/**
 * BMP_CFG_TABLE_* defines config db tables.
 */
#define BMP_CFG_DB_NAME            "CONFIG_DB"
#define BMP_CFG_TABLE_NEI          "bgp_neighbor_table"
#define BMP_CFG_TABLE_RIB_IN       "bgp-rib-in-table"
#define BMP_CFG_TABLE_RIB_OUT      "bgp-rib-out-table"

/**
 * \class   RedisManager
 *
 * \brief   RedisManager class for openbmpd
 * \details
 *      Encapsulate redis operation in this class instance.
 */
class RedisManager {

public:
    /***********************************************************************
     * Constructor for class
     ***********************************************************************/
    RedisManager();

    /*********************************************************************//**
     * Destructor for class
     ***********************************************************************/
    ~RedisManager();

    /***********************************************************************
     * Setup logger for this class
     *
     * \param [in] logPtr     logger pointer
     * \param [in] client     client pointer
     ***********************************************************************/
    void Setup(Logger *logPtr, BMPListener::ClientInfo *client);

    /**
    * DisconnectBMP
    *
    * \param [in] N/A
    */
    void DisconnectBMP();

    /**
    * ExitRedisManager
    *
    * \param [in] N/A
    */
    void ExitRedisManager();

    /**
     * Reset all Tables once FRR reconnects to BMP, this will not disable table population
     *
     * \param [in] N/A
     */
    void ResetAllTables();

    /**
     * Reset ResetBMPTable, this will flush redis
     *
     * \param [in] table    Reference to table name BGP_NEIGHBOR_TABLE/BGP_RIB_OUT_TABLE/BGP_RIB_IN_TABLE
     */
    bool ResetBMPTable(const std::string & table);

    /**
     * WriteBMPTable
     *
     * \param [in] table            Reference to table name
     * \param [in] key              Reference to various keys list
     * \param [in] fieldValues      Reference to field-value pairs
     */
    bool WriteBMPTable(const std::string& table, const std::vector<std::string>& keys, const std::vector<swss::FieldValueTuple> fieldValues);

    /**
    * SubscriberWorker, thread main for redis table subcriber.
    *
    * \param [in] table       table name to be subscribed.
    */
    void SubscriberWorker(const std::string& table);

    /**
     * ReadBMPTable, there will be dedicated thread be launched inside and monitor corresponding redis table.
     *
     * \param [in] tables     table names to be subscribed.
     */
    bool ReadBMPTable(const std::vector<std::string>& tables);

    /**
     * RemoveEntityFromBMPTable
     *
     * \param [in] table            Reference to table name
     * \param [in] args             Reference to various keys
     */
    bool RemoveEntityFromBMPTable(const std::vector<std::string>& keys);

    /**
     * Enable specific Table
     *
     * \param [in] table    Reference to table name
     */
    bool EnableTable(const std::string & table);

    /**
     * Enable BGP_Neighbor* Table
     *
     * \param [in] table    Reference to table name
     */
    bool DisableTable(const std::string & table);


    /**
     * Get Key separator for deletion
     *
     * \param [in] N/A
     */
    std::string GetKeySeparator();

private:
    swss::DBConnector stateDb_;
    std::string separator_;
    Logger *logger;
    std::unordered_set<std::string> enabledTables_;
    std::vector<std::shared_ptr<std::thread>> threadList_;
    bool exit_;
    BMPListener::ClientInfo *client_;
};


#endif /* RedisManager_H_ */
