#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"
int count = 0;
#define req std::shared_ptr<example_interfaces::srv::AddTwoInts::Request>
#define resp std::shared_ptr<example_interfaces::srv::AddTwoInts::Response>
#define serv rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr
using namespace std::chrono_literals;   

class Publisher : public rclcpp::Node{
    public:
        Publisher() : Node("publisher") {
            p = this->create_publisher<std_msgs::msg::String>("my_topic", 10);
            auto timer_callback =
            [this]() -> void {
                this->callback();
            };
            timer_ = this->create_wall_timer(50ms, timer_callback);
           
        }

    private:
        void callback()
        {
            auto message = std_msgs::msg::String();
            message.data = "Hello, world! " + std::to_string(count++);
            RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
            p->publish(message);
        }
        rclcpp::TimerBase::SharedPtr timer_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr p;
};
class Publisher2 : public rclcpp::Node{
    public:
        Publisher2() : Node("publisher2") {
            p = this->create_publisher<std_msgs::msg::String>("my_topic2", 10);
            auto timer_callback =
            [this]() -> void {
                this->callback();
            };
            timer_ = this->create_wall_timer(10ms, timer_callback);
           
        }

    private:
        void callback()
        {}
        rclcpp::TimerBase::SharedPtr timer_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr p;
};

void ServerCall(const req request, resp response){
    RCLCPP_INFO(rclcpp::get_logger("service in"), "Got request");
    response->sum = request->a + request->b;
    RCLCPP_INFO(rclcpp::get_logger("service out"), "sending back response:", (long int)response->sum);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::executors::MultiThreadedExecutor executor;
  auto pub = std::make_shared<Publisher>();
  auto pub2 = std::make_shared<Publisher2>();
  std::shared_ptr<rclcpp::Node> server_node = rclcpp::Node::make_shared("Service_Node");
  serv service = server_node->create_service<example_interfaces::srv::AddTwoInts>("Calc_Service", &ServerCall); 
  executor.add_node(pub);
  executor.add_node(pub2);
  executor.add_node(server_node);
  executor.spin();

  //rclcpp::spin(std::make_shared<Publisher>());
  rclcpp::shutdown();
  return 0;
}
