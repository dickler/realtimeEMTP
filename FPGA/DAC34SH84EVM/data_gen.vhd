----------------------------------------------------------------------------------
-- Company: 
-- Engineer:       Lance Simms 
-- 
-- Create Date:    07/04/15
-- Design Name: 
-- Module Name:    clocked_led - Behavioral 
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
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity data_gen is
    Port ( rst_clk_rx   : in  std_logic;                    -- reset signal synchronized with clk_rx
           clk_rx       : in  std_logic;                    -- operational clock
           sync         : out std_logic;
           set_ddr      : out std_logic;
           GPIO_DIP     : in std_logic_vector(7 downto 0);
           GPIO_SW_C    : in std_logic;
           led_o        : out std_logic_vector(7 downto 0);  -- drives to LEDs
           a_data            : out std_logic_vector(15 downto 0);
           b_data            : out std_logic_vector(15 downto 0);
           c_data            : out std_logic_vector(15 downto 0);
           d_data            : out std_logic_vector(15 downto 0)
           );
end data_gen;


architecture Behavioral of data_gen is
    signal comp_c: integer range 0 to 65535 := 0;
  begin
    
    data_gen_proc: process (clk_rx) 
    
      variable cnt : integer range 0 to 65535 := 0;
      variable step : integer range 0 to 255 := 0;
      variable sync_cnt: integer range 0 to 80 := 7;
      variable c_cnt: integer range 0 to 65535 := 0;
      
      variable d_cnt: integer range 0 to 40 := 0;                                                
      variable   led_buf : std_logic_vector(7 downto 0);                
      variable   a_buf : std_logic_vector(15 downto 0);
      variable   c_buf : std_logic_vector(15 downto 0) := (others=>'0');
      variable   d_buf : std_logic_vector(15 downto 0) := (others=>'0');
      variable   a_square_buf : std_logic_vector(15 downto 0) := (others=>'0');
      variable sync_buf : std_logic := '0';
      variable rising : std_logic := '1';
      
      begin
      
        if rising_edge(clk_rx) then                
          if (rst_clk_rx = '1') then                    
            led_o            <= (others=>'0');   
            a_data                <= (others=>'0');      
            led_buf          := (others=>'0');              
            a_buf            := (others=>'0'); 
            cnt              := 0;           
            sync_cnt         := 0;
          else                               
            
            sync_cnt              := sync_cnt+1;             
            c_cnt := c_cnt+1;   
            d_cnt := d_cnt+1;  
            
          if rising = '1' then
            cnt := cnt+step; 

            if cnt >= 65000 then
              rising := '0';  
            end if;
          end if;
          
          if rising = '0' then
            cnt := cnt - step;
            if cnt <= 300 then
              rising := '1';
            end if;
          end if; 
  
          if (sync_cnt = 8) then
                set_ddr <= '0';
                sync_cnt := 0;
                sync_buf := not sync_buf;
                a_square_buf := not a_square_buf;
          end if;
          
          if (c_cnt = comp_c) then
                c_cnt := 0;
                c_buf := not c_buf;
          end if;
          
          if (d_cnt = 32) then
                d_cnt := 0;
                d_buf := not d_buf;
          end if;                    
           
              a_buf      := std_logic_vector(to_unsigned(cnt,16));       
            
            -- outputs are toggled because DAC34SH84EVM outputs negative differential current (N)
            a_data <= not a_buf; 
            b_data <= not "1111111111111111";
            c_data <= not c_buf;
            d_data <= not "0000000000000000"; 
            sync  <= not GPIO_SW_C;
            led_o <= GPIO_DIP;
            
            step := to_integer(unsigned(GPIO_DIP));
        
          end if;                                      
          
        end if;                 
      end process data_gen_proc;
      
      comp_c <= 10 when GPIO_DIP = "00000001" else 
           20 when GPIO_DIP = "00000011" else 
           40 when GPIO_DIP = "00000111" else 
           80 when GPIO_DIP = "00001111" else
          160 when GPIO_DIP = "00011111" else
          1600 when GPIO_DIP = "00111111" else   
          3200 when GPIO_DIP = "01111111" else
          32000 when GPIO_DIP = "11111111" else
          500;  
  
end Behavioral;

