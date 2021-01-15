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

#include "xf_canny_config.h"
#include "xf_security/sha224_256.hpp"
#include "xf_security/sha1.hpp"

extern "C"
{
	void edgetracing_accel(ap_int<64>* kernel_input, ap_int<64>* kernel_output)
	{
// clang-format off
		#pragma HLS INTERFACE m_axi     port=kernel_input  offset=slave bundle=gmem3
		#pragma HLS INTERFACE m_axi     port=kernel_output  offset=slave bundle=gmem4
//		#pragma HLS INTERFACE s_axilite port=kernel_input
//		#pragma HLS INTERFACE s_axilite port=kernel_output
		#pragma HLS INTERFACE s_axilite port=return
//clang-format on


		printf("\nKernel is starting\n");


		printf("\nInput: %i\n", *kernel_input);

		*kernel_output = *kernel_input*50;




		printf("\n allocating hash input streams \n");

		ap_uint <640> input_data = 123456789; //should be 640bit (80byte) total to hash

		hls::stream <ap_uint <32>> msg_strm; //in
		hls::stream <ap_uint <64>> len_strm; //in
		hls::stream <bool> end_len_strm; //in
		hls::stream <ap_uint <256>> hash_strm; //out
		hls::stream <bool> end_hash_strm; //out

		printf("\n inserting hash input \n");
		end_len_strm.write(false);
		msg_strm.write(input_data);
		len_strm.write(80); //640 bit, 80 byte (64 = 2x32)


		end_len_strm.write(true);

		printf("\n calculating hash \n");
		xf::security::sha256<32>(msg_strm, len_strm, end_len_strm, hash_strm, end_hash_strm);

		/*
		bool emptiness;
		do
		{
			emptiness = hash_strm.empty();
			printf("\n hash emptiness:%i \n", (int)emptiness);
		}while(emptiness);
		*/

		printf("\n reading the hash \n");
		ap_uint <256> hash;
		hash = hash_strm.read();

		printf("\n done reading hash \n");

		printf("Hash after: %i", (int)hash);


		printf("\nKernel is done\n");

	}
}
