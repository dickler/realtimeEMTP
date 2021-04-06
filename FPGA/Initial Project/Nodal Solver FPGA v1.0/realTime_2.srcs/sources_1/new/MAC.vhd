library IEEE; 
use IEEE.STD_LOGIC_1164.all; 

entity MAC is
    port (
            clk : in std_logic;
            i_a : in STD_LOGIC_VECTOR(31 DOWNTO 0);
            i_b : in STD_LOGIC_VECTOR(31 DOWNTO 0);
            i_reset : in STD_LOGIC;
            accumValidA : in STD_LOGIC;
            disable : in STD_LOGIC;           
            output : out STD_LOGIC_VECTOR(31 DOWNTO 0)
);
end MAC; 

architecture SIMPLE of MAC is 

signal multValidA : std_logic := '1';
signal multValidB : std_logic := '1';
signal multResult : STD_LOGIC_VECTOR(31 DOWNTO 0);

begin 
	
accum: entity work.ACCUM(SIMPLE) 
  PORT MAP (
  clk => clk,
  s_axis_a_tdata => multResult,
  s_axis_a_tlast => i_reset,
  s_axis_a_tvalid => accumValidA,
  m_axis_result_tdata => output
);

mult: entity work.MULT(SIMPLE) 
  PORT MAP (
  clk => clk,
  s_axis_a_tdata => i_a,
  s_axis_b_tdata => i_b, 
  m_axis_result_tdata => multResult,
  s_axis_a_tvalid => multValidA,
  s_axis_b_tvalid => multValidB,
  disable         => disable  
);

	
	
end SIMPLE;