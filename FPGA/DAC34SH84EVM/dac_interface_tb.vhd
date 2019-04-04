----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 03/26/2019 02:56:09 PM
-- Design Name: 
-- Module Name: clocked_led_tb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
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

entity dac_interface_tb is
--  Port ( );
end dac_interface_tb;

architecture Behavioral of dac_interface_tb is

signal clk_pin_p : std_logic := '0';
signal clk_pin_n : std_logic := '1';
signal rst_pin : std_logic := '0';
signal led_pins : std_logic_vector(7 downto 0);
signal a_p : std_logic_vector(15 downto 0);
signal a_n : std_logic_vector(15 downto 0);
signal b_p : std_logic_vector(15 downto 0);
signal b_n : std_logic_vector(15 downto 0);
signal data_clk_p : std_logic;
signal data_clk_n : std_logic;
signal sync_p : std_logic;
signal sync_n : std_logic;
signal control : std_logic_vector(2 downto 0);
signal sf_d    : std_logic_vector(7 downto 4);

signal LCD_RS_LS,LCD_RW_LS,LCD_E_LS : std_logic;
signal LCD_DB7_LS,LCD_DB6_LS,LCD_DB5_LS,LCD_DB4_LS : std_logic;

constant clk_period : time := 20 ns;

signal GPIO_DIP_SW0 : std_logic := '1';
signal GPIO_DIP_SW1 : std_logic := '1';
signal GPIO_DIP_SW2 : std_logic := '1';
signal GPIO_DIP_SW3 : std_logic := '1';
signal GPIO_DIP_SW4 : std_logic := '1';
signal GPIO_DIP_SW5 : std_logic := '1';
signal GPIO_DIP_SW6 : std_logic := '1';
signal GPIO_DIP_SW7 : std_logic := '1';
signal GPIO_SW_C    : std_logic := '0';

begin

clocked_led : entity work.dac_interface

port map(clk_pin_p => clk_pin_p,
         clk_pin_n => clk_pin_n,
         rst_pin => rst_pin,
         GPIO_DIP_SW0 => GPIO_DIP_SW0,
         GPIO_DIP_SW1 => GPIO_DIP_SW1,
         GPIO_DIP_SW2 => GPIO_DIP_SW2,
         GPIO_DIP_SW3 => GPIO_DIP_SW3,
         GPIO_DIP_SW4 => GPIO_DIP_SW4,
         GPIO_DIP_SW5 => GPIO_DIP_SW5,
         GPIO_DIP_SW6 => GPIO_DIP_SW6,
         GPIO_DIP_SW7 => GPIO_DIP_SW7,
         GPIO_SW_C    => GPIO_SW_C,
         led_pins => led_pins,
         a_p => a_p,
         a_n => a_n,
         b_p => b_p, 
         b_n => b_n,  
         data_clk_p => data_clk_p, 
         data_clk_n => data_clk_n, 
         sync_p => sync_p,  
         sync_n => sync_n,
         LCD_RS_LS => LCD_RS_LS,
         LCD_RW_LS => LCD_RW_LS,
         LCD_E_LS => LCD_E_LS,
         LCD_DB7_LS => LCD_DB7_LS,
         LCD_DB6_LS => LCD_DB6_LS,
         LCD_DB5_LS => LCD_DB5_LS,
         LCD_DB4_LS => LCD_DB4_LS
);


clk_process :process
begin
    clk_pin_p <= '1';
    clk_pin_n <= '0';
    wait for clk_period/2;  --for 0.5 ns signal is '0'.
    clk_pin_p <= '0';
    clk_pin_n <= '1';
    wait for clk_period/2;  --for next 0.5 ns signal is '1'.
end process;

end architecture;


