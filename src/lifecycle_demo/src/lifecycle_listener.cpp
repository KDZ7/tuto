#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/string.hpp"

class LifecycleListener : public rclcpp_lifecycle::LifecycleNode
{
public:
    LifecycleListener() : rclcpp_lifecycle::LifecycleNode("lifecycle_listener")
    {
        RCLCPP_INFO(this->get_logger(), "LifecycleListener created");
    }

protected:
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_configure(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        subscriber = this->create_subscription<std_msgs::msg::String>("lifecycle_talker", 10, std::bind(&LifecycleListener::message_callback, this, std::placeholders::_1));
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_activate(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_cleanup(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        subscriber.reset();
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_shutdown(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_error(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

private:
    void message_callback(const std_msgs::msg::String::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LifecycleListener>();
    rclcpp::spin(node->get_node_base_interface());
    rclcpp::shutdown();
    return 0;
}
