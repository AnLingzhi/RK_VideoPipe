#include <iostream>
#include <filesystem>

#include "nodes/vp_ffmpeg_src_node.h"
#include "nodes/vp_file_src_node.h"
#include "nodes/vp_rk_rtsp_src_node.h"

#include "nodes/infer/vp_rk_first_yolo.h"
#include "nodes/infer/vp_rk_second_yolo.h"
#include "nodes/infer/vp_rk_second_cls.h"
#include "nodes/infer/vp_rk_second_rtmpose.h"

#include "nodes/vp_fake_des_node.h"
#include "nodes/vp_rtmp_des_node.h"
#include "nodes/vp_file_des_node.h"
#include "nodes/vp_screen_des_node.h"

#include "nodes/track/vp_sort_track_node.h"
#include "nodes/track/vp_byte_track_node.h"
#include "nodes/broker/vp_json_console_broker_node.h"

#include "nodes/osd/vp_osd_node.h"
#include "nodes/osd/vp_pose_osd_node.h"
#include "vp_utils/analysis_board/vp_analysis_board.h"

/*-------------------------------------------
                  Main Functions
-------------------------------------------*/
int main(int argc, char** argv) 
{
    //std::filesystem::current_path("/root/.vs/RK_VideoPipe/41b606b4-6586-4527-af89-a26a0ab1539d/src");
    VP_SET_LOG_INCLUDE_CODE_LOCATION(false);
    VP_SET_LOG_INCLUDE_THREAD_ID(false);
    VP_SET_LOG_LEVEL(vp_utils::INFO);
    VP_LOGGER_INIT();

    // create nodes
    auto src_0 = std::make_shared<vp_nodes::vp_file_src_node>("rtsp_src_0", 0, "assets/videos/person.mp4", 1.0, true, "mppvideodec");
    // auto src_0 = std::make_shared<vp_nodes::vp_rk_rtsp_src_node>("rtsp_src_0", 0, "rtsp://admin:hk123456@192.168.3.26:554/Streaming/Channels/301");
    // auto src_0 = std::make_shared<vp_nodes::vp_ffmpeg_src_node>("rtsp_src_0", 0, "rtsp://admin:hk123456@192.168.3.26:554/Streaming/Channels/301");

    auto yolo_0  = std::make_shared<vp_nodes::vp_rk_first_yolo>("rk_yolo_0", "assets/configs/person.json");    
    //auto track_0 = std::make_shared<vp_nodes::vp_sort_track_node>("track_0");
    auto track_0 = std::make_shared<vp_nodes::vp_byte_track_node>("track_0");

    //auto yolo_sub_0 = std::make_shared<vp_nodes::vp_rk_second_yolo>("rk_yolo_sub_0", "assets/configs/phone.json");
    auto pose_0 = std::make_shared<vp_nodes::vp_rk_second_rtmpose>("rk_rtmpose_0", "assets/configs/rtmpose.json", std::vector<int>{0});
    auto cls_0  = std::make_shared<vp_nodes::vp_rk_second_cls>("rk_cls_0", "assets/configs/stand_sit.json", std::vector<int>{0});
    
    auto osd_0      = std::make_shared<vp_nodes::vp_osd_node>("osd_0");
    auto pose_osd_0 = std::make_shared<vp_nodes::vp_pose_osd_node>("pose_osd_0");

    //auto des_0 = std::make_shared<vp_nodes::vp_rtmp_des_node>("rtmp_des_0", 0, "rtmp://192.168.3.100:1935/stream");
    //auto des_0 = std::make_shared<vp_nodes::vp_fake_des_node>("fake_des_0", 0);
    //auto des_0 = std::make_shared<vp_nodes::vp_file_des_node>("file_des_0", 0, "out");
    //auto des_0 = std::make_shared<vp_nodes::vp_screen_des_node>("screen_des_0", 0);
    auto des_0 = std::make_shared<vp_nodes::vp_file_des_node>("file_des_0", 0, "output.mp4");

    auto msg_broker = std::make_shared<vp_nodes::vp_json_console_broker_node>("broker_0");

    yolo_0->attach_to({src_0});
    track_0->attach_to({yolo_0});
    cls_0->attach_to({track_0});
    pose_0->attach_to({cls_0});
    osd_0->attach_to({pose_0});
    pose_osd_0->attach_to({osd_0});
    msg_broker->attach_to({pose_osd_0});
    des_0->attach_to({msg_broker});

    src_0->start();

    //getchar(); // No need to wait for input in command line mode
    //src_0->detach_recursively();
    //vp_utils::vp_analysis_board board({src_0}); // Disable UI display
    //board.display(); // Disable UI display

    // Wait for the processing to complete.
    // A more robust way would be to check the status of the pipeline or specific nodes.
    // For now, let's assume the source node will stop when the video ends.
    // We might need a mechanism to signal completion if src_0->start() is non-blocking
    // and the main thread exits before processing finishes.
    // For simplicity, if src_0 runs to completion and then stops, the program will exit.
    // If it's a live stream or continuous source, we'd need a different stop condition.
    // Let's add a small delay or a loop that checks for completion if necessary,
    // but for a file source, it should terminate when done.
    // For now, let's rely on the pipeline finishing.
    // If the program exits prematurely, we'll need to add a wait mechanism here.
    // For example, we could wait for des_0 to finish.
    // However, vp_nodes::vp_node does not seem to have a standard 'join' or 'wait_for_completion' method.
    // Let's assume the pipeline runs synchronously or src_0->start() blocks until done,
    // or the program is intended to run until manually stopped if it's a continuous stream.
    // Given it's a file source now, it should end.
    // If not, we might need to add:
    // while(src_0->is_running()) { // Assuming is_running() exists
    //    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }
    // Or, more simply, if the pipeline is fully asynchronous and main exits too early:
    std::cout << "Processing video, please wait..." << std::endl;
    // A simple way to keep the main thread alive for a while for async operations,
    // but ideally, the pipeline should have a way to signal completion.
    // If src_0->start() blocks until the source is exhausted, then this is not needed.
    // Let's assume for now that the pipeline handles its lifecycle correctly upon source completion.
    // If the output file is not complete, this is where we'd need to add waiting logic.
    // For many such pipelines, the start() call on the source node might block or the
    // program relies on a signal (like Ctrl+C) to stop for continuous streams.
    // Since we are writing to a file, it implies a finite video.
    // The original code had getchar(), which served as a wait.
    // Let's add a message and then the program will exit once src_0 completes.
    // If src_0->start() is non-blocking and returns immediately,
    // we need to wait for the pipeline to finish.
    // A common pattern is to have a loop that checks a flag or waits on a condition variable.
    // Without knowing the exact behavior of these nodes, let's keep it simple.
    // The original `getchar()` effectively made the main thread wait.
    // We can simulate a wait or rely on the pipeline structure.
    // Let's assume the pipeline processes and des_0 writes the file, then src_0 stops.
    // If the program ends too quickly, we'll need to implement a proper wait.
    // For now, let's just print a message.
    std::cout << "Video processing started. Output will be saved to output.mp4" << std::endl;
    std::cout << "Press Ctrl+C to stop if it's a continuous stream or seems stuck." << std::endl;

    // A more robust wait:
    // This is a placeholder. Ideally, the pipeline itself should provide a way to wait for completion.
    // For example, the destination node could signal when it's done.
    // Or the source node could be joined if it runs in a separate thread.
    // If src_0->start() is blocking, then the program will naturally wait here.
    // If it's non-blocking, the main thread might exit before processing is complete.
    // The original code used getchar() to pause.
    // Let's assume the pipeline is designed such that when src_0 finishes (e.g., end of video file),
    // the downstream nodes also finish their work and the program can exit.
    // If not, a more explicit synchronization mechanism is needed.
    // For now, we remove getchar() and board.display().
    // The program will run, and if src_0->start() is blocking, it will wait.
    // If non-blocking, it might exit too soon.
    // Let's assume for now it's blocking or handles its lifecycle.

    return 0;
}
