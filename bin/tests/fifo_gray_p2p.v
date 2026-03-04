//* @brief test
module fifo_gray_p2p
#( parameter DW = 32/** data width **/,  AWg = 4, AWs = 2)
(clk_r, clk_w, rst, clr, re, we, din, dout, empty, full, diff_rd, diff_wr) ;
// ------------ Port declarations --------- //
input clk_r;
input clk_w;
input clr;     //* must be in clk_r domen and long enough to meet on  clk_w
input re;	/** read enable **/
input rst;
input we;
input [DW-1:0] din;
output empty;
output full;
output [AWg:0] diff_rd;
output [AWg:0] diff_wr;
output [DW-1:0] dout;

// ----------- Signal declarations -------- //

wire [DW-1:0] dout,dout_a2pg;
wire full, empty ;

wire       re_a2pg, empty_a2pg,full_a2ps;


//---------------FIFO_ A2P gray + sync--------------------------
fifo_pgray #(.DW(DW),.AW(AWg),.R_EMPTY(0),.R_FULL(1),.R_DAT(0))
FIFO_A2Pg
(
	.clk_r   ( clk_r      ),
	.clk_w   ( clk_w      ),
	.clr     ( clr        ),
	.diff_wr ( diff_wr    ), .diff_rd(diff_rd),
	.din     ( din        ),
	.dout    (  dout_a2pg ),
	.empty   ( empty_a2pg ),
	.full    (  full      ),//unused
	.re      (    re_a2pg ),
	.rst     ( rst        ),
	.we      ( we         )
);

fifo_s #( .DW(DW) ,  .AW(AWs),.R_DAT(0))
FIFO_A2Ps
(
	.rst_n  ( rst       ),
	.clk    ( clk_r      ),
	.clr    ( clr        ),
	.diff   (            ),
	.din    ( dout_a2pg  ),
	.dout   ( dout       ),
	.empty  ( empty      ),
	.full   ( full_a2ps  ),
	.re     ( re         ),
	.we     (   re_a2pg  )
);

assign    re_a2pg = ~empty_a2pg  & ~full_a2ps;
endmodule
