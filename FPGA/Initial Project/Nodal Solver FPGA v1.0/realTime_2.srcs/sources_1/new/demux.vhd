
-- Function: 32-bit demultiplexer

library ieee;
  use ieee.std_logic_1164.all;
  use ieee.numeric_std.all;

entity demux is
port (
--  clk          : in std_logic;
  data         : in std_logic_vector(31 downto 0);
  -- address      : in  std_logic;
  address      : in  std_logic_vector(2 downto 0);
  vk      : out std_logic_vector(31 downto 0);
  vm      : out std_logic_vector(31 downto 0)
);
end demux;

architecture behaviour of demux is

begin

  demux_a_to_y: process (address,data)
  begin
--  if rising_edge(clk) then
      -- if address = '1'  then
      if address = "001" or address = "010"  then
        vk <= data;
      else
        vm <= data;
    end if;
--    end if;
  end process;

end behaviour;