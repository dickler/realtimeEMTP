library IEEE;
use IEEE.STD_LOGIC_1164.ALL; 
use ieee.numeric_std.all;

entity MULT is

    port ( 
    clk : in std_logic;
    s_axis_a_tdata : in STD_LOGIC_VECTOR(31 DOWNTO 0); 
    s_axis_b_tdata : in STD_LOGIC_VECTOR(31 DOWNTO 0); 
    m_axis_result_tdata : out STD_LOGIC_VECTOR(31 DOWNTO 0);
    s_axis_a_tvalid : in std_logic;
    s_axis_b_tvalid : in std_logic;
    disable : in std_logic
    ); 
    
end MULT; 

architecture SIMPLE of MULT is 

    
COMPONENT floating_point_1
      PORT (
        aclk : IN STD_LOGIC;
        s_axis_a_tvalid : IN STD_LOGIC;
        s_axis_a_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        s_axis_b_tvalid : IN STD_LOGIC;
        s_axis_b_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        m_axis_result_tvalid : OUT STD_LOGIC;
        m_axis_result_tdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
      );
    END COMPONENT;
      
    signal m_axis_result_tvalid : STD_LOGIC;
    signal m_axis_result_tdataAUX : STD_LOGIC_VECTOR(31 DOWNTO 0);       
--    signal s_axis_a_tvalid : STD_LOGIC := '1'; 
--    signal s_axis_b_tvalid : STD_LOGIC := '1';      
       
    begin
    
your_instance_name : floating_point_1
      PORT MAP (
        aclk => clk,
        s_axis_a_tvalid => s_axis_a_tvalid,
        s_axis_a_tdata => s_axis_a_tdata,
        s_axis_b_tvalid => s_axis_b_tvalid,
        s_axis_b_tdata => s_axis_b_tdata,
        m_axis_result_tvalid => m_axis_result_tvalid,
        m_axis_result_tdata => m_axis_result_tdataAUX
      );

m_axis_result_tdata <= m_axis_result_tdataAUX when disable = '0' else
               (others =>'0');
end SIMPLE; 
