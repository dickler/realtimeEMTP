----------------------------------------------------------------------------------
-- Company:        
-- Engineer:       Felipe Dicler
-- 
-- Create Date:    20/03/18
-- Design Name: 
-- Module Name:    dac_interface - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
------------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

library UNISIM;
use UNISIM.VComponents.all;

entity networkSolver is

    Port ( clk_pin_p     : in  STD_LOGIC;
           clk_pin_n     : in  STD_LOGIC;
           rst_pin       : in  STD_LOGIC;
           GPIO_DIP_SW0  : in  STD_LOGIC;
           GPIO_DIP_SW1  : in  STD_LOGIC;
           GPIO_DIP_SW2  : in  STD_LOGIC;
           GPIO_DIP_SW3  : in  STD_LOGIC;
           GPIO_DIP_SW4  : in  STD_LOGIC;
           GPIO_DIP_SW5  : in  STD_LOGIC;
           GPIO_DIP_SW6  : in  STD_LOGIC;
           GPIO_DIP_SW7  : in  STD_LOGIC;
           GPIO_SW_C     : in STD_LOGIC;
           led_pins      : out STD_LOGIC_VECTOR (7 downto 0);
           a_p            : out STD_LOGIC_VECTOR (15 downto 0);
           a_n            : out STD_LOGIC_VECTOR (15 downto 0);
           b_p            : out STD_LOGIC_VECTOR (15 downto 0);
           b_n            : out STD_LOGIC_VECTOR (15 downto 0);
           data_clk_p     : out STD_LOGIC;
           data_clk_n     : out STD_LOGIC;
           sync_p         : out STD_LOGIC;
           sync_n         : out STD_LOGIC;
           LCD_RS_LS      : out STD_LOGIC;
           LCD_RW_LS      : out STD_LOGIC;
           LCD_E_LS       : out STD_LOGIC;
           LCD_DB7_LS     : out STD_LOGIC;
           LCD_DB6_LS     : out STD_LOGIC;
           LCD_DB5_LS     : out STD_LOGIC;
           LCD_DB4_LS     : out STD_LOGIC                                                                                                                                                                   
         );
end networkSolver;


architecture Behavioral of networkSolver is

    --
    -- module definitions
    --    
    component data_gen is
        Port ( rst_clk_rx         : in  std_logic;
               clk_rx             : in  std_logic;
               sync               : out  std_logic;
               set_ddr            : out  std_logic;
               GPIO_DIP           : in   std_logic_vector(7 downto 0);
               GPIO_SW_C          : in   std_logic;    
               led_o              : out std_logic_vector(7 downto 0);
               a_data                  : out std_logic_vector(15 downto 0);
               b_data                  : out std_logic_vector(15 downto 0);
               c_data                  : out std_logic_vector(15 downto 0);
               d_data            : out std_logic_vector(15 downto 0)
        );
    end component data_gen;  
 
    component meta_harden is
        Port ( clk_dst            : in  std_logic;
               rst_dst            : in  std_logic;
               signal_src         : in  std_logic;
               signal_dst         : out std_logic
        );
    end component meta_harden;
    
component clk_phase
    port
     (
      clk_rx            : out    std_logic; -- single ended output
      clk_rx_90         : out    std_logic; -- single ended output 90 degrees shifted  
      reset             : in     std_logic; -- reset
      locked            : out    std_logic; -- control signal
      clk_in1_p         : in     std_logic; -- p clock input (differential)
      clk_in1_n         : in     std_logic  -- n clock input (differential)
     );
    end component;

COMPONENT c_shift_ram_16 -- 7 bits 10 clocks register
  PORT (
    D : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END COMPONENT;
        

    -- clock and controls
    signal rst_i, rst_clk_rx                     : std_logic := 'U';
    signal clk_i, clk_rx,clk_rx_90               : std_logic := 'U';
    
    signal rxd_i                  : std_logic := 'U';
    signal led_o                  : std_logic_vector(7 downto 0) := (others=>'U');
    --signal T                      : std_logic := '0'; -- Tri-state OBUFDS value
    
    signal a_data                      : std_logic_vector(15 downto 0) := (others=>'0');
    signal b_data                      : std_logic_vector(15 downto 0) := (others=>'0');
    signal c_data                      : std_logic_vector(15 downto 0) := (others=>'0');
    signal d_data                      : std_logic_vector(15 downto 0) := (others=>'0');
  
    signal a                      : std_logic_vector(15 downto 0) := (others=>'0');
    signal b                      : std_logic_vector(15 downto 0) := (others=>'0');  
    
    signal lcd_control                : std_logic_vector(2 downto 0); -- LCD control bits
    signal lcd_data                   : std_logic_vector(7 downto 4); -- LCD data bits (high nibble)
    
    signal sync                   : std_logic := '0';             -- DAC sync signal
    
    signal locked                 : std_logic; -- MMCT output bit
    
    signal set_ddr                : std_logic := '0'; 
    signal GPIO_DIP               : std_logic_vector(7 downto 0); 
    
    
   

begin

    --
    -- define the buffers for the incoming data, clocks, and control
    IBUF_rst_i0:    IBUF    port map (I=>rst_pin, O=>rst_i);
    
    
    -- OBUFTDS: Differential 3-state Output Buffer
      OBUFTDS_inst : OBUFTDS
    generic map (
    IOSTANDARD => "LVDS")
    port map (
    O => data_clk_p, -- Diff_p output (connect directly to top-level port)
    OB => data_clk_n, -- Diff_n output (connect directly to top-level port)
    I => clk_rx_90, -- Buffer input
    T => '0' -- 3-state enable input
    );
    
    -- define the buffers for the outgoing data
    b_i_OBUFTDS_inst: for i in 0 to 15 generate
          b_i_OBUFTDS: OBUFTDS   generic map (
    IOSTANDARD => "LVDS")
    port map (
    O => b_p(i), -- Diff_p output (connect directly to top-level port)
    OB => b_n(i), -- Diff_n output (connect directly to top-level port)
    I => b(i), -- Buffer input
    T =>'0' -- 3-state enable input
    );
       end generate;
       
-- define the buffers for the outgoing data
      a_i_OBUFTDS_inst: for i in 0 to 15 generate
            a_i_OBUFTDS: OBUFTDS   generic map (
      IOSTANDARD => "LVDS")
      port map (
      O => a_p(i), -- Diff_p output (connect directly to top-level port)
      OB => a_n(i), -- Diff_n output (connect directly to top-level port)
      I => a(i), -- Buffer input
      T => '0'-- 3-state enable input
      );
         end generate;         
       
-- ODDR: Output Double Data Rate Output Register with Set, Reset
      ODDR_inst: for i in 0 to 15 generate
       ODDR_inst_channel_a : ODDR
       generic map(
       DDR_CLK_EDGE => "SAME_EDGE", -- "OPPOSITE_EDGE" or "SAME_EDGE"
       INIT => '0', -- Initial value for Q port ('1' or '0')
       SRTYPE => "SYNC") -- Reset Type ("ASYNC" or "SYNC")
       port map (
       Q => a(i), -- 1-bit DDR output
       C => clk_rx, -- 1-bit clock input
       CE => '1', -- 1-bit clock enable input
       D1 => a_data(i), -- 1-bit data input (positive edge)
       D2 => b_data(i), -- 1-bit data input (negative edge)
       R => '0', -- 1-bit reset input
       S => '0' -- 1-bit set input
       );
       end generate;
       -- End of ODDR_inst instantiation
       
-- ODDR: Output Double Data Rate Output Register with Set, Reset

     ODDR_inst_channel_b: for i in 0 to 15 generate
      ODDR_inst_channel_b : ODDR
      generic map(
      DDR_CLK_EDGE => "SAME_EDGE", -- "OPPOSITE_EDGE" or "SAME_EDGE"
      INIT => '1', -- Initial value for Q port ('1' or '0')
      SRTYPE => "SYNC") -- Reset Type ("ASYNC" or "SYNC")
      port map (
      Q => b(i), -- 1-bit DDR output
      C => clk_rx, -- 1-bit clock input
      CE => '1', -- 1-bit clock enable input
      D1 => c_data(i), -- 1-bit data input (positive edge)
      D2 => d_data(i), -- 1-bit data input (negative edge)
      R => '0', -- 1-bit reset input
      S => '0' -- 1-bit set input
      );
      end generate;
      -- End of ODDR_inst instantiation       
       
      sync_OBUFTDS_inst : OBUFTDS
generic map (
IOSTANDARD => "LVDS")
port map (
O => sync_p, -- Diff_p output (connect directly to top-level port)
OB => sync_n, -- Diff_n output (connect directly to top-level port)
I => sync, -- Buffer input
T => '0' -- 3-state enable input
);


clock_phase : clk_phase
   port map ( 
  -- Clock out ports
   clk_rx         => clk_rx,  
   clk_rx_90 => clk_rx_90,
  -- Status and control signals                
   reset => rst_clk_rx,
   locked => locked,
   -- Clock in ports
   clk_in1_p => clk_pin_p,
   clk_in1_n => clk_pin_n
 );

-- define the buffers for the outgoing data
OBUF_led_ix: for i in 0 to 7 generate
      OBUF_led_i: OBUF port map (I=>LED_o(i), O=>LED_pins(i));
   end generate;

-- instantiate a metastability hardener for the incoming reset
-- meta_harden_rst_i0: meta_harden port map (rst_dst=>'0', clk_dst=>clk_rx, signal_src=>rst_i, signal_dst=>rst_clk_rx);

-- instantiate the data generator module
--data_generator: data_gen port map (
--                               rst_clk_rx        => rst_clk_rx,
--                               clk_rx             => clk_rx,
--                               sync               => sync,
--                               set_ddr            => set_ddr,
 --                              GPIO_DIP           => GPIO_DIP,
 --                              GPIO_SW_C          => GPIO_SW_C,
 --                              led_o              => led_o,
 --                              a_data             => a_data,
 --                              b_data             => b_data,
 --                              c_data             => c_data,
   --                            d_data             => d_data 
   --                           );
                              
-- instantiate the main Nodal Analysis Solver
nodalSolver: entity work.main(behavioral)
        port map (
                    clk => clk_rx,
                    cmd => GPIO_SW_C,
                    output1 => a_data,
                    output2 => b_data,
                    output3 => c_data,
                    output4 => d_data
                    );
                    
ledsRegister : c_shift_ram_16
  PORT MAP (
    D => GPIO_DIP,
    CLK => clk_rx,
    Q => led_o
  );
                    

-- instantiate the LCD controller
LCD: entity work.lcd_controller(lcd_control_arch) 
      port map ( 
               clk         => clk_rx,
               rst         => rst_clk_rx,
               lcd_control => lcd_control,
               lcd_data    => lcd_data
       );                              
 
 GPIO_DIP <= GPIO_DIP_SW7&GPIO_DIP_SW6&GPIO_DIP_SW5&GPIO_DIP_SW4&GPIO_DIP_SW3&GPIO_DIP_SW2&GPIO_DIP_SW1&GPIO_DIP_SW0;
 
 LCD_RS_LS <= lcd_control(2);
 LCD_RW_LS <= lcd_control(1);
 LCD_E_LS  <= lcd_control(0);
 
 LCD_DB7_LS <= lcd_data(7);
 LCD_DB6_LS <= lcd_data(6);
 LCD_DB5_LS <= lcd_data(5);
 LCD_DB4_LS <= lcd_data(4);
        
end Behavioral;

