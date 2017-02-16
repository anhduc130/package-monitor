module DE1_SOC_PWM(
	input 		          		clk_in,
	input				            control_in, 
	
	output 						   pwm_out 
);



//LEFT = 00001100110011011
//VERTICAL = 00010011001100110

//=======================================================
//  REG/WIRE declarations
//=======================================================
	wire clk; 
	wire PWM_out; 

	reg [16:0] control_out;
	
	assign pwm_out = PWM_out; 
	
	always @ (posedge clk_in) begin 
		if(control_in == 1'b1) 
			control_out = 17'b00001100110011011;
		if(control_in == 1'b0)
			control_out = 17'b00010011001100110; 
	end 
	

//=======================================================
//  PLL Instantiation 
//=======================================================
PLL PLL_inst(
					.refclk(clk_in),	//Input Clock 
					.rst(1'b0), 			//Reset 
					.outclk_0(clk)       //5 MHz Output Clock 
);


//=======================================================
//  PWM Controller 
//=======================================================
PWM_Controller PWM_inst_1 (
	.clk (clk),
	.PWM_CW (control_out),
	.PWM_out (PWM_out)
);





endmodule
