/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2017-2020, Waterplus http://www.6-robot.com
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the WaterPlus nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  FOOTPRINTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/
/* @author Zhang Wanjie                                             */

#include <ros/ros.h>
#include <amr_waypoint_tools/SaveWaypoints.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "waypoint_saver");
    ros::NodeHandle nh;
    ros::NodeHandle private_nh("~"); // 创建私有NodeHandle用于获取参数

    ros::ServiceClient cliSave = nh.serviceClient<amr_waypoint_tools::SaveWaypoints>("waypoint/save_waypoints");
    amr_waypoint_tools::SaveWaypoints srvS;

    // 默认保存路径
    std::string default_save_path;
    char const* home = getenv("HOME");
    default_save_path = home;
    default_save_path += "/waypoints.xml";

    // 从参数服务器获取文件路径参数
    std::string save_file_path;
    private_nh.param<std::string>("save_file_path", save_file_path, default_save_path);

    // 命令行参数优先级高于launch文件参数
    for(int i=1; i<argc; i++)
    {
        if(!strcmp(argv[i], "-f"))
        {
            if(++i < argc)
            {
                save_file_path = argv[i];
                ROS_INFO("Using command line specified path: %s", save_file_path.c_str());
            }
        }
    }

    srvS.request.filename = save_file_path;

    if (cliSave.call(srvS))
    {
        ROS_INFO("Save waypoints to the file : %s", srvS.request.filename.c_str());
    }
    else
    {
        ROS_ERROR("Failed to call service save_waypoints");
    }

    return 0;
}