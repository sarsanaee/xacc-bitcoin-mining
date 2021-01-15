/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//J: still a lot more junk left over. Basically removed some image-processing specific functions and commented out one of the kernels
#include "common/xf_headers.hpp"
#include "xf_canny_config.h"
#include "xcl2.hpp"

typedef unsigned char NMSTYPE;


int main(int argc, char** argv)
{
	std::vector<cl::Device> devices = xcl::get_xil_devices();
	cl::Device device = devices[0];
	cl::Context context(device);

	std::string device_name = device.getInfo<CL_DEVICE_NAME>();
	std::string binaryFile = xcl::find_binary_file(device_name, "krnl_canny");
	cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
	devices.resize(1);
	cl::Program program(context, devices, bins);
	cl::Kernel krnl_miner(program, "edgetracing_accel");

	cl::Event event_sp_edge;
	cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

	int64_t kernel_input = 5;
	int64_t kernel_output;

	cl::Buffer buf_inputToKernel(context, CL_MEM_READ_ONLY, 64);
	cl::Buffer buf_resultFromKernel(context, CL_MEM_WRITE_ONLY, 64);

	q.enqueueWriteBuffer(buf_inputToKernel, CL_TRUE, 0, 64, &kernel_input);

	// Set the kernel arguments
	krnl_miner.setArg(0, buf_inputToKernel);
	krnl_miner.setArg(1, buf_resultFromKernel);




	printf("before kernel");
	// Launch the kernel
	q.enqueueTask(krnl_miner, NULL, &event_sp_edge);
	clWaitForEvents(1, (const cl_event*)&event_sp_edge);

	printf("after kernel");

	q.enqueueReadBuffer(buf_resultFromKernel, CL_TRUE, 0, 64, &kernel_output);
	q.finish();

	printf("\nHost read: %d\n", kernel_output);

	//clReleaseProgram(program);

	return 0;
}
