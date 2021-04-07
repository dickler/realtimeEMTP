----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 04.03.2021 05:49:58
-- Design Name: 
-- Module Name: s_switch - Behavioral
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
use IEEE.Numeric_Std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity s_switch is
    Port ( clk : in STD_LOGIC;
           cmd : in STD_LOGIC;
           rst : in STD_LOGIC;
           s_out : out STD_LOGIC);
end s_switch;

architecture Behavioral of s_switch is

signal s : std_logic := '0';
signal counter : std_logic_vector(15 downto 0) := (others => '0');

-- states variables
type stateType is (
    -- main states 
	start,
    switching
);
signal state : stateType := start;

begin

process(clk) -- state machine for switch controll
    begin
        if clk'event and clk='1' then
            if rst = '0' then
            case state is 
                when start =>
                    counter <= (others => '0');
                    s <= '0';
                    if cmd = '1' then
                        state <= switching;
                    end if;
                when switching =>
                    if unsigned(counter) = 4999 then -- IGBT switch each 50 clocks (10 kHz) 
                        s <= not s;
                        counter <= (others => '0');
                    else
                        counter <= std_logic_vector(unsigned(counter) + 1);
                    end if;
            end case;
                
            else -- reset case
                counter <= (others => '0');
                state <= start;
                s <= '0';
            end if;
        end if;
    end process;

s_out <= s;

end Behavioral;
