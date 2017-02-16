`timescale 1ps / 1ps

module counter (
	clk,				//Counter clock
	counter_out   			 // 17 bit output from the counter
);

	input clk;			// clock declared as an input port
	output reg [16:0] counter_out;  // counter_out declared as an 8 bit output register
	
	always @(posedge clk)
		begin
			counter_out <= #1 counter_out + 1'b1;
		end
		
endmodule				// identifies the end of the module