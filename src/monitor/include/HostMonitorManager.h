/* -------------------------------------------------------------------------- */
/* Copyright 2002-2019, OpenNebula Project, OpenNebula Systems                */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

#ifndef HOST_MONITOR_MANAGER_H_
#define HOST_MONITOR_MANAGER_H_

#include "MonitorDriverMessages.h"
#include "HostRPCPool.h"

#include <vector>

class Template;
class VectorAttribute;

template<typename E, typename D>
class DriverManager;

class VMRPCPool;
class OneMonitorDriver;
class UDPMonitorDriver;
class TCPMonitorDriver;
class MonitorDriver;

class Monitor;

/**
 *  This class controls the monitor actions and logic of OpenNebula hosts
 *
 */
class HostMonitorManager
{
public:
    HostMonitorManager(HostRPCPool *hp, VMRPCPool *vmp,
            const std::string& addr, unsigned int port, unsigned int threads,
            const std::string& driver_path,
            int timer_period,
            int monitor_interval_host);

    ~HostMonitorManager();

    OneMonitorDriver* get_oned_driver() const { return oned_driver; }

    //--------------------------------------------------------------------------
    //  Driver Interface
    //--------------------------------------------------------------------------
    /**
     *
     */
    int load_monitor_drivers(const std::vector<const VectorAttribute*>& config);

    /**
     *  Start the monitor manager drivers to process events
     */
    int start(std::string& error);

    //--------------------------------------------------------------------------
    //  Management / Monitor Interface
    //--------------------------------------------------------------------------
    /**
     *  Start the monitor agent/ or active monitor the host
     *   @param oid the host id
     */
    void start_host_monitor(int oid);

    /**
     *  Stop the monitor agent/ or stop monitor the host
     *   @param oid the host id
     */
    void stop_host_monitor(int oid);

    /**
     *  Updates the information of the given host. If it does not exist it is
     *  added to the pool
     *    @param oid host id
     *    @param xml the XML representation of the host
     */
    void update_host(int oid, const std::string &xml);

    /**
     *  Updates the last monitoring for the host
     *    @param oid host id
     */
    void update_last_monitor(int oid);

    /**
     *  Remove host from the pool
     *    @param oid host id
     */
    void delete_host(int oid);

    /**
     *  Sets the monitor information of the host. It notifies oned if needed.
     *    @param oid host id
     *    @param tmpl monitoring template
     */
    void monitor_host(int oid, bool result, const Template &tmpl);

    /**
     *  Sets the monitor information of the VM.
     *    @param oid VM id
     *    @param result
     *    @param tmpl monitoring template
     */
    void monitor_vm(int oid,
                    const std::string &deploy_id,
                    const Template &tmpl);

    /**
     *  Receive start monitor failure/success from driver
     *    @param oid host id
     */
    void start_monitor_failure(int oid);

    void start_monitor_success(int oid);

    /**
     *  This function is executed periodically to update host monitor status
     */
    void timer_action();

private:
    using driver_manager_t = DriverManager<MonitorDriverMessages, MonitorDriver>;

    driver_manager_t* driver_manager;

    OneMonitorDriver* oned_driver;

    UDPMonitorDriver* udp_driver;

    TCPMonitorDriver* tcp_driver;

    HostRPCPool* hpool;

    VMRPCPool* vmpool;

    unsigned int threads;

    /**
     *  Timer period for timer_action loop
     */
    int timer_period;

    /**
     *  Host monitoring interval
     */
    int monitor_interval_host;

    /**
     *  Time in seconds to expire a monitoring action (5 minutes)
     */
    static const time_t monitor_expire;

    void start_host_monitor(const HostRPCPool::HostBaseLock& host);

    void stop_host_monitor(const HostRPCPool::HostBaseLock& host);
};

#endif //HOST_MONITOR_MANAGER_H_