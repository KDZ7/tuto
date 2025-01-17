#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/string.hpp"

class LifecycleTalker : public rclcpp_lifecycle::LifecycleNode
{
public:
    LifecycleTalker() : rclcpp_lifecycle::LifecycleNode("lifecycle_talker")
    {
        RCLCPP_INFO(this->get_logger(), "LifecycleTalker created");
        this->declare_parameter<int64_t>("period_ms", 1000);
    }

protected:
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_configure(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        publisher = this->create_publisher<std_msgs::msg::String>("lifecycle_talker", 10);
        period_ms = this->get_parameter("period_ms").get_value<int64_t>();
        period_ms = period_ms < 0 ? 1000 : period_ms;
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_activate(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        publisher->on_activate();
        timer = this->create_wall_timer(std::chrono::milliseconds(period_ms), std::bind(&LifecycleTalker::timer_callback, this));
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        publisher->on_deactivate();
        timer.reset();
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_cleanup(const rclcpp_lifecycle::State &state) override
    {
        RCLCPP_INFO(this->get_logger(), "From: %s to: %s", state.label().c_str(), this->get_current_state().label().c_str());
        publisher.reset();
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
    void timer_callback()
    {
        auto message = std_msgs::msg::String();
        message.data = "c^.^) ... Hello, I'm a lifecycle talker !";
        publisher->publish(message);
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
    }

    rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>::SharedPtr publisher;
    rclcpp::TimerBase::SharedPtr timer;
    int64_t period_ms;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LifecycleTalker>();
    rclcpp::spin(node->get_node_base_interface());
    rclcpp::shutdown();
    return 0;
}