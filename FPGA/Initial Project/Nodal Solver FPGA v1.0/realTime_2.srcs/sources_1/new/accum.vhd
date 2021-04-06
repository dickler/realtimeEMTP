library IEEE;
use IEEE.STD_LOGIC_1164.ALL; 
use ieee.numeric_std.all;

entity ACCUM is -- begin entity, name it NAND2

    port ( -- beginning of port statement
    clk : in std_logic;
    s_axis_a_tdata : in STD_LOGIC_VECTOR(31 DOWNTO 0); -- A_H is an input of type STD_LOGIC
    s_axis_a_tlast : in STD_LOGIC;
    s_axis_a_tvalid : in std_logic;
    m_axis_result_tdata : out STD_LOGIC_VECTOR(31 DOWNTO 0)
    ); -- parenthesis to close port statement
    
end ACCUM; -- end of the entity part

architecture SIMPLE of ACCUM is -- begin architecture, hook to NAND2
    -- declaration area, empty here
    
COMPONENT floating_point_0
      PORT (
        aclk : IN STD_LOGIC;
        s_axis_a_tvalid : IN STD_LOGIC;
        s_axis_a_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        s_axis_a_tlast : IN STD_LOGIC;
        m_axis_result_tvalid : OUT STD_LOGIC;
        m_axis_result_tdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
        m_axis_result_tlast : OUT STD_LOGIC
      );
    END COMPONENT;
      
    signal m_axis_result_tvalid : STD_LOGIC;  
    signal m_axis_result_tlast : STD_LOGIC := '1';       
               
    begin
    
your_instance_name : floating_point_0
      PORT MAP (
        aclk => clk,
        s_axis_a_tvalid => s_axis_a_tvalid,
        s_axis_a_tdata => s_axis_a_tdata,
        s_axis_a_tlast => s_axis_a_tlast,
        m_axis_result_tvalid => m_axis_result_tvalid,
        m_axis_result_tdata => m_axis_result_tdata,
        m_axis_result_tlast => m_axis_result_tlast
      );
    

    
end SIMPLE; -- end of architecture part; name
-- must be name of architecture