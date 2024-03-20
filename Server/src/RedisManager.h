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

#include <hiredis/hiredis.h>
#include <string>
#include <list>
#include <map>


using namespace boost::xpressive;

/**
 * \class   RedisManager
 *
 * \brief   RedisManager class for openbmpd
 * \details
 *      Encapsulate redis operation in this class instance.
 */
class RedisManager {
public:
    /**
     * BMP_TABLE_* defines the default table name prefix
     */
    #define BMP_TABLE_NEI              "BGP_NEIGHBOR_TABLE"
    #define BMP_TABLE_RIB_IN           "BGP_RIB_IN_TABLE"
    #define BMP_TABLE_RIB_OUT          "BGP_RIB_OUT_TABLE"
    #define BMP_TABLE_NEI_PREFIX       "BGP_NEIGHBOR"
    #define BMP_TABLE_NEI_KEYS         "KEYS BGP_NEIGHBOR*"
    #define BMP_TABLE_RIB_IN_KEYS      "KEYS BGP_RIB_IN_TABLE*"
    #define BMP_TABLE_RIB_OUT_KEYS     "KEYS BGP_RIB_OUT_TABLE*"
   

    /*********************************************************************//**
     * Get singleton instance for class
     ***********************************************************************/
    static RedisManager& getInstance();

    /***********************************************************************
     * Constructor for class
     ***********************************************************************/
    RedisManager();

    /***********************************************************************
     * Setup logger for this class
     *
     * \param [in] logPtr     logger pointer
     ***********************************************************************/
    void Setup(Logger *logPtr);
    
    /**
     * Reset all Tables once FRR reconnects to BMP, this will not disable table population
     *
     * \param [in] N/A
     */
    bool ResetAllTables();

    /**
     * Reset BGP_Neighbor* Table
     *
     * \param [in] N/A
     */
    bool ResetBGPNeighborTable();

    /**
     * Reset BGP_RIB_IN* Table
     *
     * \param [in] N/A
     */
    bool ResetBGPRibInTable();

    /**
     * Reset BGP_RIB_OUT* Table
     *
     * \param [in] N/A
     */
    bool ResetBGPRibOutTable();

    /**
     * WriteBGPNeighborTable
     *
     * \param [in] neighbor  Reference to neighbor address
     * \param [in] field     Reference to the specific field name
     * \param [in] value     Reference to the value to be set
     */
    bool WriteBGPNeighborTable(const std::string& neighbor, const std::string& field, const std::string& value);

    /**
     * WriteBGPInRibTable
     *
     * \param [in] neighbor  Reference to neighbor address
     * \param [in] nlri      Reference to nlri
     * \param [in] field     Reference to the specific field name
     * \param [in] value     Reference to the value to be set
     */
    bool WriteBGPRibInTable(const std::string& neighbor, const std::string& nlri, const std::string& field, const std::string& value);

    /**
     * WriteBGPOutRibTable
     *
     * \param [in] neighbor  Reference to neighbor address
     * \param [in] nlri      Reference to nlri
     * \param [in] field     Reference to the specific field name
     * \param [in] value     Reference to the value to be set
     */
    bool WriteBGPRibOutTable(const std::string& neighbor, const std::string& nlri, const std::string& field, const std::string& value);

    /**
     * Enable BGP_Neighbor* Table
     *
     * \param [in] N/A
     */
    bool EnableBGPNeighborTable();

    /**
     * Enable BGP_Neighbor* Table
     *
     * \param [in] N/A
     */
    bool DisableBGPNeighborTable();

    /**
     * Enable BGP_RIB_IN* Table
     *
     * \param [in] N/A
     */
    bool EnableBGPRibInTable();

    /**
     * Enable BGP_RIB_IN* Table
     *
     * \param [in] N/A
     */
    bool DisableBGPRibInTable();

    /**
     * Enable BGP_RIB_OUT* Table
     *
     * \param [in] N/A
     */
    bool EnableBGPRibOutTable();

    /**
     * Enable BGP_RIB_OUT* Table
     *
     * \param [in] N/A
     */
    bool DisableBGPRibOutTable();

private:
    /*********************************************************************//**
     * Destructor for class
     ***********************************************************************/
    ~RedisManager();

    /**
     * Connect to redis and maintain the context
     *
     * \param [in] N/A
     */
    bool ConnectRedis();

    /**
     * Reset BGP_Neighbor* Table
     *
     * \param [in] N/A
     */
    bool ResetBGPNeighborTable();

    /**
     * Reset BGP_RIB_IN* Table
     *
     * \param [in] N/A
     */
    bool ResetBGPRibInTable();

    /**
     * Reset BGP_RIB_OUT* Table
     *
     * \param [in] N/A
     */
    bool ResetBGPRibOutTable();

private:
    redisContext* redis_;
    Logger *logger_;
    bool connected_;
    bool neiTableEnable_;
    bool ribInTableEnable_;
    bool ribOutTableEnable_;
};


#endif /* RedisManager_H_ */
