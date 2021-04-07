----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 04/04/2019 09:16:23 AM
-- Design Name: 
-- Module Name: lcd_controller - Behavioral
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
use IEEE.NUMERIC_STD.ALL;


entity lcd_controller is
    Port ( clk : in STD_LOGIC;  --built for 10MHz
           rst : in STD_LOGIC;
           lcd_control : out STD_LOGIC_VECTOR(2 downto 0);
           lcd_data : out STD_LOGIC_VECTOR(7 downto 4));
end lcd_controller;

architecture lcd_control_arch of lcd_controller is

signal sequence : std_logic_vector(0 to 307);
signal adr : natural range 0 to 76 := 0;
signal timer : unsigned(18 downto 0) := (others => '1');    -- about 100ms after powerup
signal hi_lo : std_logic := '0';    --transmitting high or low byte
signal lcd_rw,lcd_rs,lcd_e : std_logic;


constant line_1 : STD_LOGIC_VECTOR (127 downto 0) := "00100000"&"00100000"&"00100000"&"00100000"&"01100101"&"01101101"&"01100010"&"01110010"&"01100001"&"01010010"&"01010100"&"00100000"&"00100000"&"00100000"&"00100000"&"00100000"; 
constant line_2 : STD_LOGIC_VECTOR (127 downto 0) := "00100000"&"00100000"&"00100000"&"01110100"&"01100101"&"01100011"&"01101000"&"01101110"&"01101111"&"01101100"&"01101111"&"01100111"&"01111001"&"00100000"&"00100000"&"00100000";
--https://www.rapidtables.com/convert/number/ascii-hex-bin-dec-converter.html
begin

--             0       1    2       3    4       5     6       7    8     9    10    11-42     43   44     45-76
sequence <= "0010" & "00101000"  & "00001100" & "00000110" & "00000001" & "00000010" & line_1 & "11000000" & line_2;
--           4bit    4Bit+Setup   Display On  Entry Mode    Clear         Home         Data    line 2 adr   Data

lcd_rw <= '0';
lcd_rs <= '0' when (adr < 11 or adr = 43 or adr = 44) else '1';
lcd_data <= sequence(adr*4 to adr*4+3);
lcd_e <= '1' when timer < 32768 and timer > 32735 else '0';

process(clk)
begin
    if(rising_edge(clk)) then
        if rst = '1' then
            timer <= (others=>'1');
            adr <= 0;
            hi_lo <= '0';
        else
            if timer = 0 or (timer = 32671 and hi_lo = '1') then
                timer(14 downto 0) <= (others=>'1'); -- 2^15 = 32768
                hi_lo <= not hi_lo;
            else
                timer <= timer-1;
            end if;
            if timer = 64 or (timer = 32703 and hi_lo = '1') then
                if adr < 76 then
                    adr <= adr+1;
                else
                    adr <= 9;
                end if;
            end if;            
        end if;
    end if;
end process;

lcd_control <= lcd_rs & lcd_rw & lcd_e;

end lcd_control_arch;

