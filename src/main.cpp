#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <getopt.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}
void ResetSimulator(uWS::WebSocket<uWS::SERVER> ws) {

  std::string msg = "42[\"reset\",{}]";
  ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);

}

int main(int argc, char* argv[])
{
  uWS::Hub h;

  PID pid_steer;
  PID pid_thrott;
  // TODO: Initialize the pid variable.
  std::vector<double> steer;
  double kp_steer = 0;
  double kd_steer = 0;
  double ki_steer = 0;

  // Twiddle algorithm run in python enable
  bool auto_tune = false;
  bool started = false;

  // starting and ending point of optimization
  int frame_start= 0;
  int frame_end = 1000;
  int i=0;
  int option;

  // get parameters from commandline
  // a => auto_tune with twiddle from python
  // s => steering pid parameters kp, ki, kd, => example: -s 5,6,7,
  // f => starting and ending point from auto_tunning => example -f 500,1000

  while((option = getopt(argc, argv, "as:f:")) !=-1) {
      switch (option) {
          case 'a': {
            auto_tune = true;
            std::cout<<"External auto tune"<<std::endl;
            break;
        }
          case 's': {


            char delimiter = ',';
            std::string acc = "";
            std::string input = optarg;
            int ind = 0;
            for(unsigned int i = 0; i < input.size(); i++)
            {
                if(input[i] == delimiter)
                {
                    if(ind==0) {kp_steer = std::atof(acc.c_str());}
                    if(ind==1) {ki_steer = std::atof(acc.c_str());}
                    if(ind==2) {kd_steer = std::atof(acc.c_str());}
                    ind ++;
                    acc = "";
                }
                else
                    acc += input[i];
            }
            break;
        }
           case 'f':
           {


               char delimiter = ',';
               std::string acc = "";
               std::string input = optarg;
               int ind = 0;
               for(unsigned int i = 0; i < input.size(); i++)
               {
                   if(input[i] == delimiter)
                   {
                       if(ind==0) {frame_start = std::atof(acc.c_str());}
                       if(ind==1) {frame_end = std::atof(acc.c_str());}
                       ind ++;
                       acc = "";
                   }
                   else
                       acc += input[i];
               }
               break;
           }

      }
  }
  std::cout<<"kp: "<<kp_steer<<" ki: "<< ki_steer<<" kd: "<< kd_steer<<std::endl;
    std::cout<<"start: "<<frame_start<<" end: "<< frame_end<<std::endl;
  pid_steer.Init(kp_steer, ki_steer, kd_steer);
  //pid_thrott.Init(kp_thrott, kd_thrott, ki_thrott);

  h.onMessage([&auto_tune, &i, &pid_steer, &pid_thrott, &frame_start, &frame_end, &started](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */
          pid_steer.UpdateError(cte);
          steer_value = - pid_steer.TotalError() / deg2rad(25.0);
          steer_value = std::min(1.0, std::max(-1.0, steer_value));
          //std::cout<< i<<std::endl;
          i++;

          //communication with python if auto_tune enabled
          if (auto_tune){
              if ((i==frame_start)&&(!started)){
                  //reset error and frame
                  pid_steer.Reset();
                  std::cout << "started" << std::endl;
                  started = true;
                  i=0;
              }
              if (( i>frame_end) || ((fabs(cte) > 4.5)&&(started))){
                  // expects parameters to try
                  double Kp, Kd, Ki;
                  std::cout << "$input@" << std::endl;
                  // broadcast the last error from the previous parameters
                  std::cout << "$" <<pid_steer.GetAvgError(i)<<"@"<<std::endl;
                  std::cin >> Kp >> Kd >> Ki;
                  //resets everything
                  pid_steer.SetParams(Kp, Kd, Ki);
                  pid_steer.Reset();
                  ResetSimulator(ws);
                  i = 0;
                  started = false;
              }
        }
          // DEBUG

          //std::cout << "CTE: " << cte << " Steering Value: " << steer_value <<" Angle: "<<angle<< std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
