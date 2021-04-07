----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 12.03.2021 16:09:41
-- Design Name: 
-- Module Name: debug_gen - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity debug_gen is
    Port ( clk : in STD_LOGIC;
           b_out : out STD_LOGIC_VECTOR(15 DOWNTO 0)
);
end debug_gen;

architecture Behavioral of debug_gen is

signal counter : std_logic_vector(15 downto 0) := (others => '0');

begin
process(clk) -- state machine for switch controll
    begin
        if clk'event and clk='1' then
            if counter = "1111111111111111" then
                counter <= (others => '0');
            else
                counter <= std_logic_vector(unsigned(counter) + 1);
            end if;
        end if;
    end process;
b_out <= counter;                

end Behavioral;
