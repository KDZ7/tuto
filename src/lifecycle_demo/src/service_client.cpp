#include "rclcpp/rclcpp.hpp"
#include "rclcpp/parameter_client.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"
#include "lifecycle_msgs/srv/get_state.hpp"
#include <iostream>
#include <chrono>

class ServiceClient : public rclcpp::Node
{
public:
    ServiceClient() : Node("service_client")
    {
        lfc_talker_change_state = this->create_client<lifecycle_msgs::srv::ChangeState>("lifecycle_talker/change_state");
        lfc_listener_change_state = this->create_client<lifecycle_msgs::srv::ChangeState>("lifecycle_listener/change_state");
        lfc_talker_get_state = this->create_client<lifecycle_msgs::srv::GetState>("lifecycle_talker/get_state");
        lfc_listener_get_state = this->create_client<lifecycle_msgs::srv::GetState>("lifecycle_listener/get_state");
        lfc_talker_parameters_client = std::make_shared<rclcpp::AsyncParametersClient>(this, "lifecycle_talker");
    }

    void do_action(const std::string &lfc_name, const std::string &str_action, const int64_t &period_ms = 0)
    {
        if (str_action == "configure")
        {
            RCLCPP_INFO(this->get_logger(), "Configuring %s ...", lfc_name.c_str());
            if (!do_set_parameter("period_ms", period_ms))
                return;
            if (!do_change_state(lfc_name, lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE))
                return;
        }
        else if (str_action == "activate")
        {
            RCLCPP_INFO(this->get_logger(), "Activating %s ...", lfc_name.c_str());
            if (!do_change_state(lfc_name, lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE))
                return;
        }
        else if (str_action == "deactivate")
        {
            RCLCPP_INFO(this->get_logger(), "Deactivating %s ...", lfc_name.c_str());
            if (!do_change_state(lfc_name, lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE))
                return;
        }
        else if (str_action == "cleanup")
        {
            RCLCPP_INFO(this->get_logger(), "Cleaning up %s ...", lfc_name.c_str());
            if (!do_change_state(lfc_name, lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP))
                return;
        }
        else if (str_action == "shutdown")
        {
            RCLCPP_INFO(this->get_logger(), "Shutting down %s ...", lfc_name.c_str());
            std::string current_state = do_get_state(lfc_name);
            if (current_state == "unconfigured")
            {
                if (!do_change_state(lfc_name, lifecycle_msgs::msg::Transition::TRANSITION_UNCONFIGURED_SHUTDOWN))
                    return;
            }
            else
                RCLCPP_WARN(this->get_logger(), "You can only shutdown from unconfigured state");
        }
        else if (str_action == "state")
        {
            RCLCPP_INFO(this->get_logger(), "Getting state of %s ...", lfc_name.c_str());
            if (do_get_state(lfc_name).empty())
                return;
        }
        else if (str_action == "info")
        {
            RCLCPP_INFO(this->get_logger(), "Getting info of %s ...", lfc_name.c_str());
            if (do_get_info(lfc_name).empty())
                return;
        }
        RCLCPP_INFO(this->get_logger(), "Done !");
    }

private:
    bool do_set_parameter(const std::string &param_name, const int64_t &param_value)
    {
        if (!lfc_talker_parameters_client->wait_for_service(std::chrono::seconds(5)))
        {
            RCLCPP_ERROR(this->get_logger(), "Parameter service not available");
            return false;
        }
        auto future = lfc_talker_parameters_client->set_parameters({rclcpp::Parameter(param_name, param_value)});
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future, std::chrono::seconds(5)) != rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to set parameter %s", param_name.c_str());
            return false;
        }

        auto results = future.get();
        for (const auto &result : results)
        {
            if (!result.successful)
            {
                RCLCPP_ERROR(this->get_logger(), "Failed to set parameter '%s' : %s", param_name.c_str(), result.reason.c_str());
                return false;
            }
            RCLCPP_INFO(this->get_logger(), "Parameter '%s' set to %ld", param_name.c_str(), param_value);
        }
        return true;
    }

    bool do_change_state(const std::string &lfc_name, const uint8_t &transition)
    {
        auto request = std::make_shared<lifecycle_msgs::srv::ChangeState::Request>();
        request->transition.id = transition;
        if (lfc_name == "lifecycle_talker")
        {
            if (!lfc_talker_change_state->wait_for_service(std::chrono::seconds(5)))
            {
                RCLCPP_ERROR(this->get_logger(), "Service %s not available", lfc_name.c_str());
                return false;
            }
            auto future = lfc_talker_change_state->async_send_request(request);
            if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future, std::chrono::seconds(5)) != rclcpp::FutureReturnCode::SUCCESS)
            {
                RCLCPP_ERROR(this->get_logger(), "Failed to change state of %s", lfc_name.c_str());
                return false;
            }
        }
        else if (lfc_name == "lifecycle_listener")
        {
            if (!lfc_listener_change_state->wait_for_service(std::chrono::seconds(5)))
            {
                RCLCPP_ERROR(this->get_logger(), "Service %s not available", lfc_name.c_str());
                return false;
            }
            auto future = lfc_listener_change_state->async_send_request(request);
            if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future, std::chrono::seconds(5)) != rclcpp::FutureReturnCode::SUCCESS)
            {
                RCLCPP_ERROR(this->get_logger(), "Failed to change state of %s", lfc_name.c_str());
                return false;
            }
        }
        return true;
    }

    std::string do_get_state(const std::string &lfc_name)
    {
        auto request = std::make_shared<lifecycle_msgs::srv::GetState::Request>();
        auto client = lfc_name == "lifecycle_talker" ? lfc_talker_get_state : lfc_name == "lifecycle_listener" ? lfc_listener_get_state
                                                                                                               : nullptr;
        if (!client)
        {
            RCLCPP_ERROR(this->get_logger(), "Invalid lifecycle node name");
            return "";
        }
        if (!client->wait_for_service(std::chrono::seconds(5)))
        {
            RCLCPP_ERROR(this->get_logger(), "Service %s not available", lfc_name.c_str());
            return "";
        }
        auto future = client->async_send_request(request);
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future, std::chrono::seconds(5)) == rclcpp::FutureReturnCode::SUCCESS)
        {
            auto result = future.get();
            RCLCPP_INFO(this->get_logger(), "Current state of %s: %s (%d)", lfc_name.c_str(), result->current_state.label.c_str(), result->current_state.id);
            return result->current_state.label.c_str();
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to get state of %s", lfc_name.c_str());
            return "";
        }
        return "";
    }

    std::string do_get_info(const std::string &lfc_name)
    {
        std::string current_state = do_get_state(lfc_name);
        std::vector<std::string> possible_transitions;
        if (current_state == "unconfigured")
        {
            possible_transitions = {"configure", "shutdown"};
        }
        else if (current_state == "inactive")
        {
            possible_transitions = {"activate", "cleanup"};
        }
        else if (current_state == "active")
        {
            possible_transitions = {"deactivate"};
        }
        else if (current_state == "finalized")
        {
            possible_transitions = {"none"};
        }
        else
        {
            possible_transitions = {"unknown state"};
        }
        std::stringstream ss;
        for (const auto &transition : possible_transitions)
            ss << " [ " << transition << " ] ";
        RCLCPP_INFO(this->get_logger(), "Possible transitions: %s", ss.str().c_str());

        return ss.str();
    }

    std::shared_ptr<rclcpp::AsyncParametersClient> lfc_talker_parameters_client;
    rclcpp::Client<lifecycle_msgs::srv::ChangeState>::SharedPtr lfc_talker_change_state;
    rclcpp::Client<lifecycle_msgs::srv::ChangeState>::SharedPtr lfc_listener_change_state;
    rclcpp::Client<lifecycle_msgs::srv::GetState>::SharedPtr lfc_talker_get_state;
    rclcpp::Client<lifecycle_msgs::srv::GetState>::SharedPtr lfc_listener_get_state;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ServiceClient>();
    std::string lfc_name, action;
    int64_t period_ms;

    std::cout << "Lifecycle CLI Service Client" << std::endl;
    std::cout << "Usage: [lfc_name] [action] [period_ms]" << std::endl;
    std::cout << "lfc_name: lifecycle_talker | lifecycle_listener" << std::endl;
    std::cout << "action: configure | activate | deactivate | cleanup | shutdown | state | info" << std::endl;
    std::cout << "period_ms: period in milliseconds (only for configure action)" << std::endl;
    std::cout << "Example: lifecycle_talker configure 1000" << std::endl;

    while (rclcpp::ok())
    {
        std::cout << "\n CLI Service Client > ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit" or input == "quit")
            break;
        std::istringstream iss(input);
        iss >> lfc_name >> action;
        if (action == "configure")
            iss >> period_ms;
        std::cout << "lfc_name: " << lfc_name << ", action: " << action << ", period_ms: " << period_ms << std::endl;
        node->do_action(lfc_name, action, period_ms);
    }
    rclcpp::shutdown();
    return 0;
}
