--* @brief Parameterized AXI-Lite Register Bank
--* @description This module implements a configurable register bank for control status
--* @date 2023-10-25
--* @status stable
--* @warning Reset is asynchronous and active low
--* @todo Add ECC support for register storage

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity axi_reg_bank is
    generic (
        REG_COUNT     : integer := 16;                    --* Number of 32-bit registers in bank
        DATA_WIDTH    : integer := 32;                    --* Data width must be power of 2
        ADDR_WIDTH    : integer := 4;                     --* Address width calculated from REG_COUNT
        INIT_PATTERN  : std_logic_vector(31 downto 0) := X"DEADBEEF"; --* Default init value
        ENABLE_DEBUG  : boolean := true                   --* optional Debug mode enable
    );
    port (
        clk, rst_n       : in  std_logic;
        data_in, addr    : in  std_logic_vector(7 downto 0);
        result           : out std_logic_vector(15 downto 0);
        axi_aclk      : in  std_logic;                    --* System clock signal
        axi_aresetn   : in  std_logic;                    --* Asynchronous active-low reset
        axi_awaddr    : in  std_logic_vector(ADDR_WIDTH-1 downto 0); --* Write address
        axi_awvalid   : in  std_logic;                    --* Write address valid
        axi_awready   : out std_logic;                    --* Write address ready
        axi_wdata     : in  std_logic_vector(DATA_WIDTH-1 downto 0); --* Write data bus
        axi_wstrb     : in  std_logic_vector(DATA_WIDTH/8-1 downto 0); --* Write strobe
        axi_wvalid    : in  std_logic;                    --* Write data valid
        axi_wready    : out std_logic;                    --* Write data ready
        axi_bresp     : out std_logic_vector(1 downto 0); --* OKAY or SLVERR response
        axi_bvalid    : out std_logic;                    --* Write response valid
        axi_bready    : in  std_logic;                    --* Write response ready
        axi_araddr    : in  std_logic_vector(ADDR_WIDTH-1 downto 0); --* Read address
        axi_arvalid   : in  std_logic;                    --* Read address valid
        axi_arready   : out std_logic;                    --* Read address ready
        axi_rdata     : out std_logic_vector(DATA_WIDTH-1 downto 0); --* Read data bus
        axi_rresp     : out std_logic_vector(1 downto 0); --* Check for error response
        axi_rvalid    : out std_logic;                    --* Read data valid
        axi_rready    : in  std_logic;                    --* Read data ready
        -- Debug Interface
        debug_en      : in  std_logic;                    --* Enable debug output
        debug_data    : out std_logic_vector(31 downto 0) --* Connect to internal state
    );
end entity axi_reg_bank;

architecture rtl of axi_reg_bank is
    -- Internal implementation
begin
end architecture rtl;