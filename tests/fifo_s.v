////	FIFO Single Clock	////
`timescale 1ns / 10ps 
/*Description
===========

I/Os
----
rst_n	low active, asynchronous master reset
clr	high active synchronous reset 
re	read enable, synchronous, high active
we	write enable, synchronous, high active
din	Data Input
dout	Data Output

full	Indicates the FIFO is full  (registered output)
empty	Indicates the FIFO is empty (registered output)
diff    Indicates the number of busy words

-------------- precousens! -------------------------------------------
This design assumes you will do appropriate status checking externally.

IMPORTANT ! writing while the FIFO is full or reading while the FIFO is
empty will place the FIFO in an undefined state.

*/

module fifo_s
#(	parameter    DW = 32,  AW = 4,R_DAT =0) //data_width,addr_width, registered data used

	(clk, rst_n, clr, re, we, din, dout, empty, full,  diff) ;	
	
	input	clk;
	input	rst_n;
	input	clr;
	input	[DW-1:0] din;
	output	[DW-1:0] dout;
	input	we;
	input	re;
	output	full;
	output	empty;
	output	[AW:0] diff;

	//
	// Local Wires
	//	
	reg	[AW:0]	wp;
	reg	[AW:0]	rp;
	reg 	[AW:0] diff;
	reg	 empty,full;

// functions in design-----------------------------
function [AW:0] incr_1;
input [AW:0] b;
begin
 incr_1 = b + { {AW{1'b0}}, 1'b1};
end
endfunction

function [AW:0] decr_1;
input [AW:0] b;
begin
 decr_1 = b  -{ {AW{1'b0}}, 1'b1};
end
endfunction
//  end of functions -------------------------------


// -------- Component instantiations -------//
//here  can be  inserted  yours DPRAM
//instead  of registered memory
/*
ddr_dpram #( .DW(DW), .AW(AW))
DPRAM_s (
	.AA(wp[AW-1:0]),
	.AB(rp[AW-1:0]),
	.CLKA(clk),
	.DA(din),
	.QB(dout),
	.WENA(we)
);

*/
//-----------------end of instantiation-------------------


//-----------------registered memory-------------------
reg  [DW-1:0]    mem [0:(1<<AW)-1];  // fifo registers

wire [AW-1:0] AA= wp[AW-1:0]; 
wire [AW-1:0] AB= rp[AW-1:0]; 
wire [DW-1:0] doutw;
reg  [DW-1:0] doutr;

always @( posedge clk)
       if (we)  mem[AA]  <= din;
assign doutw =  mem[AB];

always @( posedge clk) doutr <= doutw;

assign dout =  (R_DAT == 1) ? doutr:  doutw ;

//------------      FLAGS ------------------------
wire [AW:0]	wp_pl = incr_1(wp);

	always @(posedge clk or negedge rst_n)
		if(!rst_n)	wp <= {AW +1{1'b0}};
		else if(clr)	wp <= {AW +1{1'b0}};		
		else if(we)	wp <= incr_1(wp);
				
	always @(posedge clk  or negedge rst_n)
		if(!rst_n)	rp <= {AW +1{1'b0}};
		else if(clr)	rp <= {AW +1{1'b0}};			
		else if(re)	rp <= incr_1(rp);
				
	always @(posedge clk  or negedge rst_n)
		     if(!rst_n)	    full <= 1'b0;
		else if (clr || re) full <= 1'b0;
		else if (we)	    full <=  (wp_pl[AW-1:0] == rp[AW-1:0]) & (wp_pl[AW]!=rp[AW]);
				
	always @(posedge clk  or negedge rst_n)
		if(!rst_n)	empty <=  1'b1;
		else if (clr)	empty <=  1'b1;
		else if (we)	empty <=  1'b0;
		else if (re)	empty <=  wp == incr_1(rp);

	always @(posedge clk  or negedge rst_n)
		     if  (!rst_n)	 diff <= {AW +1{1'b0}};
		else if  (clr)	         diff <= {AW +1{1'b0}};
		else if  ( we && !re)	 diff <= incr_1(diff);
		else if  (!we &&  re)	 diff <= decr_1(diff);
			
//------------------ Sanity Check ----------------------------
  // for test purposes only
	// synopsys translate_off
// pragma coverage off
	always @(posedge clk)
		if(we & full)
			$display("%m WARNING: Writing while fifo is FULL (%t)",$time);
	always @(posedge clk)
		if(re & empty)
			$display("%m WARNING: Reading while fifo is EMPTY (%t)",$time);

// pragma coverage on
	// synopsys translate_on

endmodule
