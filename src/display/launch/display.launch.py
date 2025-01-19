from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution, Command
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    urdf_path = PathJoinSubstitution([
        FindPackageShare('description'),
        'ur10e',
        'ur10e.urdf.xacro'
        ])
    rviz_config = PathJoinSubstitution([
        FindPackageShare('display'), 
        'config', 
        'rviz.yaml'
        ])
    robot_description = Command(['xacro ', urdf_path])
    ld = []
    robot_state_publisher = Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[{'robot_description': robot_description}]
        )
    joint_state_publisher_gui = Node(
            package='joint_state_publisher_gui',
            executable='joint_state_publisher_gui',
            name='joint_state_publisher_gui',
            output='screen'
        )
    rviz2 = Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen',
            arguments=['-d', rviz_config]
        )
    ld.extend([robot_state_publisher, joint_state_publisher_gui, rviz2])
    return LaunchDescription(ld)