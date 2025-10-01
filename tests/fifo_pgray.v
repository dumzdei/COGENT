
module fifo_pgray
#( parameter DW = 2,  AW = 5, R_EMPTY =0,R_FULL =0,R_DAT =0)
 (clk_r, clk_w, rst, clr, re, we, din, dout, empty, full, diff_rd, diff_wr) ;

localparam  MS =1; //don't change !!! 

// ------------ Port declarations --------- //
input clk_r;
input clk_w;
input clr;     //must be in clk_r domen and long enough to meet on  clk_w
input re;
input rst;
input we;
input [DW-1:0] din;
output empty;
output full;
output [AW:0] diff_rd;
output [AW:0] diff_wr;
output [DW-1:0] dout;

// ----------- Signal declarations -------- //
wire [AW-1:0] AA;
wire [AW-1:0] AB;

reg  [AW  :0]    rpB;
reg  [AW  :0]    rpBg;
reg  [AW  :0]    rpBg_ss,rpBg_ms;

reg  [AW  :0]    wpA;
reg  [AW  :0]    wpAg;
reg  [AW  :0]    wpAg_ss, wpAg_ms;

wire [AW  :0]   wpAg_s,rpBg_s;


reg  clr_ms,clr_s; // clear on clk_w!	
wire full, empty ;


// functions in design-------------------------------
function [AW:0] bin_to_gray;
input [AW:0] b;
begin
 bin_to_gray = b ^ (b>>1);
end
endfunction


function [AW:0] gray_to_bin;
input [AW:0] g;
reg   [AW:0] b;
integer      i;
begin
 for( i=0; i<=AW; i=i+1 ) b[i] = ^(g>>i);
 gray_to_bin = b;
end
endfunction
//  end of functions in design-------------------------------

////////////////////////////////////////////////////////////////////

//	Write Pointer in Gray and Binary  Code
wire [AW  :0] wpA_pl = wpA  + {{AW{1'b0}}, 1'b1};
wire [AW  :0] wpAg_pl = bin_to_gray(wpA_pl);

always @( posedge clk_w or negedge rst)
     if(!rst)   begin wpA   <={AW+1{1'b0}}; wpAg <={AW+1{1'b0}}; end
else if (clr_s) begin wpA   <={AW+1{1'b0}}; wpAg <={AW+1{1'b0}}; end
else if (we)    begin wpA   <= wpA_pl;      wpAg <= wpAg_pl; end

////////////////////////////////////////////////////////////////////

//	Read Pointer in Gray and Binary Code
wire [AW  :0] rpB_pl  = rpB + {{AW{1'b0}}, 1'b1};
wire [AW  :0] rpBg_pl = bin_to_gray(rpB_pl);

always @( posedge clk_r or negedge rst)
		     if (!rst) begin rpB <= {AW+1{1'b0}}; rpBg <= {AW+1{1'b0}};end
		else if (clr)  begin rpB <= {AW+1{1'b0}}; rpBg <= {AW+1{1'b0}};end
		else if (re)   begin rpB <= rpB_pl;       rpBg <= rpBg_pl;     end

//----------------Synchronization Logic----------------------------// 

	always @(posedge clk_w or negedge rst)
		if (!rst) begin  clr_ms <=1'b0; clr_s <= 1'b0; end
	   else           begin  clr_ms <=clr;  clr_s <= clr_ms; end 

	always @(posedge clk_w or negedge rst)
		if (!rst)   begin  rpBg_ms <={AW+1{1'b0}}; rpBg_ss <= {AW+1{1'b0}}; end
	   else if (clr_s)  begin  rpBg_ms <={AW+1{1'b0}}; rpBg_ss <= {AW+1{1'b0}}; end
	   else             begin  rpBg_ms <= rpBg;        rpBg_ss <= rpBg_ms; end
	
	always @(posedge clk_r or negedge rst)
		if (!rst) begin  wpAg_ms <= {AW+1{1'b0}};  wpAg_ss <= {AW+1{1'b0}}; end 
	   else if (clr)  begin  wpAg_ms <= {AW+1{1'b0}};  wpAg_ss <= {AW+1{1'b0}}; end 
	   else           begin  wpAg_ms <= wpAg;          wpAg_ss <= wpAg_ms; end 


assign rpBg_s = (MS==1) ? rpBg_ss :rpBg_ms;
assign wpAg_s = (MS==1) ? wpAg_ss :wpAg_ms;
//---------------gray coded  wired FULL, EMPTY Flags ---------------------//
wire              fullw;
wire              emptyw;

assign fullw =  ((rpBg_s ^ wpAg) == ((AW ==1) ? 2'b11:{2'b11, {AW-1{1'b0}}})) ? 1:0;
assign emptyw = ((wpAg_s ^ rpBg) == {AW+1{1'b0}}) ? 1:0;

//---------------binary  FULL, EMPTY Flags(wired)---------------------//
//	assign full  = (wpA[AW-1:0] == rpB_s[AW-1:0]) && (wpA[AW] != rpB_s[AW])? 1:0;
//	assign empty = (wpA_s[AW:0] == rpB[AW:0])? 1:0;

//-----------------gray coded registered flags---------------------//
reg fullr,emptyr;

always @( posedge clk_r or negedge rst)
      if( !rst) emptyr <= 1'b1; else 
      if (clr)  emptyr <= 1'b1; else
begin           emptyr <= (rpBg    == wpAg_s )| 
                     re & (rpBg_pl == wpAg_s );   
end


always @( posedge clk_w or negedge rst)
      if( !rst)   fullr <= 1'b0; else
      if (clr_s)  fullr <= 1'b0; else
begin             fullr <=  ((wpAg    ^ rpBg_s)== ((AW ==1) ? 2'b11:{2'b11, {AW-1{1'b0}}}) )  |                 
                       we & ((wpAg_pl ^ rpBg_s)== ((AW ==1) ? 2'b11:{2'b11, {AW-1{1'b0}}}) )  ;
end

/*
//-------------binary  coded registered flags-------------
always @( posedge clk_r or negedge rst)
      if( !rst) emptyr <= 1'b1; else 
      if (clr)  emptyr <= 1'b1; else
begin           emptyr <= (rpB    == wpA_s )| 
                     re & (rpB_pl == wpA_s );   
end

always @( posedge clk_w or negedge rst)
       if( !rst)  fullr <= 1'b0; else
      if (clr_s)  fullr <= 1'b0; else
begin             fullr <= (( wpA[     AW-1:0] == rpB_s[AW-1:0] ) & ( wpA[   AW] != rpB_s[AW] )) |                        
                        (we & ( wpA_pl[AW-1:0] == rpB_s[AW-1:0] ) & ( wpA_pl[AW] != rpB_s[AW] )) ;
end
*/


//--------------- Additional word counters ---------------------//
wire [AW  :0] wpA_s = gray_to_bin(wpAg_s);
wire [AW  :0] rpB_s = gray_to_bin(rpBg_s);

	assign diff_wr =  wpA   - rpB_s; 
	assign diff_rd =  wpA_s - rpB; 


// -------- Component instantiations -------//
 //binary coded address--------------------
assign AA= 	wpA[AW-1:0]; 
assign AB= 	rpB[AW-1:0]; 

// gray coded address--------------------	
//assign AA= {wpAg[AW] ^ wpAg[AW-1],wpAg[AW-2:0]}; 
//assign AB= {rpBg[AW] ^ rpBg[AW-1],rpBg[AW-2:0]};


wire [DW-1:0] doutw;
reg  [DW-1:0] doutr;

/*
ddr_dpram #( .BITS(DW) ,  .AW(AW) )
DPRAMb (
	.AA(AA),
	.AB(AB),
	.CLKA(clk_w),
	.DA(din),
	.QB(doutw),
	.WENA(we)
);
*/


reg  [DW-1:0]    mem [0:(1<<AW)-1];  // fifo data
always @( posedge clk_w)
        if (we)  mem[AA]  <= din;
assign doutw =   mem[AB];



always @( posedge clk_r) doutr <= doutw;


assign dout =  (R_DAT  ==1) ? doutr:  doutw ;
assign full  = (clr_s) ? 1'b1 :(R_FULL ==1) ? fullr:  fullw ;
assign empty = (R_EMPTY==1) ? emptyr: emptyw;


//------------------ Sanity Check ----------------------------
  // for test purposes only
	// synopsys translate_off
// pragma coverage off
	always @(posedge clk_w)
		if(we & full)
			$display("%m WARNING: Writing while fifo is FULL  Data Loss!(%t)",$time);
	always @(posedge clk_r)
		if(re & empty)
			$display("%m WARNING: Reading while fifo is EMPTY Data invalid(%t)",$time);
// pragma coverage on
	// synopsys translate_on


endmodule
