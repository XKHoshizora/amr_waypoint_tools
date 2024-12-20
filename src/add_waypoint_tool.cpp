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

#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include <amr_waypoint_tools/Waypoint.h>
#include <amr_waypoint_tools/GetWaypointByName.h>
#include "rviz/display_context.h"
#include "rviz/properties/string_property.h"
#include "add_waypoint_tool.h"

static int nWaypointCount = 0;

namespace rviz
{
    AddWaypointTool::AddWaypointTool()
    {
        shortcut_key_ = 'a';
        topic_property_ = new StringProperty( "Topic", "/waypoint/add_waypoint","The topic on which to add new waypoints.",getPropertyContainer(), SLOT( updateTopic() ), this );
    }

    AddWaypointTool::~AddWaypointTool()
    {
    }

    void AddWaypointTool::onInitialize()
    {
        PoseTool::onInitialize();
        setName( "Add Waypoint" );
        updateTopic();
    }

    void AddWaypointTool::updateTopic()
    {
        pub_ = nh_.advertise<amr_waypoint_tools::Waypoint>( topic_property_->getStdString(), 1);
        cliGetWPName = nh_.serviceClient<amr_waypoint_tools::GetWaypointByName>("/waypoint/get_waypoint_name");
    }

    void AddWaypointTool::onPoseSet(double x, double y, double theta)
    {
        std::string fixed_frame = context_->getFixedFrame().toStdString();
        tf::Quaternion quat;
        quat.setRPY(0.0, 0.0, theta);
        tf::Stamped<tf::Pose> p = tf::Stamped<tf::Pose>(tf::Pose(quat, tf::Point(x, y, 0.0)), ros::Time::now(), fixed_frame);
        geometry_msgs::PoseStamped new_pos;
        tf::poseStampedTFToMsg(p, new_pos);
        ROS_INFO("Add new waypoint: Frame:%s, Position(%.3f, %.3f, %.3f), Orientation(%.3f, %.3f, %.3f, %.3f) = Angle: %.3f\n", fixed_frame.c_str(),
            new_pos.pose.position.x, new_pos.pose.position.y, new_pos.pose.position.z,
            new_pos.pose.orientation.x, new_pos.pose.orientation.y, new_pos.pose.orientation.z, new_pos.pose.orientation.w, theta);
        amr_waypoint_tools::Waypoint new_waypoint;

        nWaypointCount ++;
        std::ostringstream stringStream;
        stringStream << nWaypointCount;
        std::string strNewName = stringStream.str();

        amr_waypoint_tools::GetWaypointByName srvN;
        srvN.request.name = strNewName;
        while (cliGetWPName.call(srvN))
        {
            nWaypointCount ++;
            std::ostringstream stringStream;
            stringStream << nWaypointCount;
            strNewName = stringStream.str();
            srvN.request.name = strNewName;
        }
        ROS_WARN("New waypoint name = %s",strNewName.c_str());
        new_waypoint.name = strNewName;

        new_waypoint.pose = new_pos.pose;
        pub_.publish(new_waypoint);
    }
}

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(rviz::AddWaypointTool,rviz::Tool)
