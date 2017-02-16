`timescale 1ps / 1ps

module PWM_Controller (
	PWM_CW,               // Ports declared
	PWM_out,					         
	clk
	);
	
	input clk;            //Port type declared
	input [16:0] PWM_CW;   // 17 bit PWM input
	
	output reg PWM_out; // 1 bit PWM output
	wire [16:0] counter_out;  // 17 bit counter output

	

	always @ (posedge clk)
	begin
			
			if (PWM_CW > counter_out)
				PWM_out <= 1;
				
			else 
				PWM_out <= 0;
	end
		
	
	counter counter_inst(
	
		.clk (clk),
		.counter_out (counter_out)
		);

endmodule 