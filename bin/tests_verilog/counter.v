/** @brief
4-bit up-counter with synchronous reset \
This module implements a basic 4-bit binary counter that increments on every
positive clock edge. The counter wraps around from 15 to 0 automatically. \
A synchronous active-low reset allows initialization to zero when rst_n is low.
*/

/** @note This counter uses synchronous reset which provides 
better timing closure and predictability. \
The initial block ensures simulation starts from known state but 
may not synthesize to hardware initialization in all tools.
*/

/** @warning If rst_n is not properly synchronized to clk, \
metastability issues may occur when reset is released near clock edge.
*/

/**
@status Verified in simulation
@author Nikolaenkov Dmitry. Email: dimanik116@gmail.com
@date 11.11.2025
*/
module simple_counter (
    input wire clk,     //* Clock, signal
    input wire rst_n,    //* Active-low synchronous reset
    output reg [3:0] q //* 4-bit counter output
);
    
    initial begin
        q = 4'b0000;
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            q <= 4'b0000;
        end else begin
            q <= q + 1;
        end
    end

endmodule
