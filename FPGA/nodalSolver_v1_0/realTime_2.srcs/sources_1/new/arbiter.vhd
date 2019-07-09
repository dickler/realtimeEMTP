--this module is an arbiter to manage modules access to ram memory

library IEEE; -- make IEEE library visible
use IEEE.STD_LOGIC_1164.all; -- open STD_LOGIC_1164 package
use IEEE.Numeric_Std.all;

entity arbiterV is
    port (  -- clock
            clk : in std_logic;
            
            -- signals used by this arbiter to decide which module has access to RAM memory
            onIVEC : in STD_LOGIC;
            onVETMAT : in STD_LOGIC;
            
            -- RAM memory access signal
            ramOutput : in STD_LOGIC_VECTOR (31 downto 0);
            
            -- request signals
            ramAddressVETMAT  : in STD_LOGIC_VECTOR (3 downto 0);
            ramInputVETMAT : in STD_LOGIC_VECTOR (31 downto 0);
            ramAddressIVEC  : in STD_LOGIC_VECTOR (3 downto 0);
            ramInputIVEC : in STD_LOGIC_VECTOR (31 downto 0);
            wr_enIVEC : in STD_LOGIC_VECTOR (0 downto 0);
            wr_enVETMAT : in STD_LOGIC_VECTOR (0 downto 0);
            
            -- outputs signals to RAM memory
            wr_en : out STD_LOGIC_VECTOR (0 downto 0) := "0";
            ramAddress  : out STD_LOGIC_VECTOR (3 downto 0) := (others=>'0');
            ramInput : out STD_LOGIC_VECTOR (31 downto 0) := (others=>'0');
            
            -- outputs signals to VETMAT and HISTVECT
            ramOutputVETMAT : out STD_LOGIC_VECTOR (31 downto 0);
            ramOutputIVEC : out STD_LOGIC_VECTOR (31 downto 0)                        
);
end arbiterV; 

architecture SIMPLE of arbiterV is 
	
begin 
-- decide which module can request a address to the RAM memory
ramAddress <= ramAddressIVEC when onIVEC = '1' else
               ramAddressVETMAT when (onIVEC = '0' and onVETMAT = '1') else
               (others=>'0');

-- decide which module has access to write in the RAM memory
ramInput <= ramInputIVEC when onIVEC = '1' else
            ramInputVETMAT when (onIVEC = '0' and onVETMAT = '1') else
            (others=>'0');

-- decide which module can request a writing in the RAM memory
wr_en <= wr_enIVEC when onIVEC = '1' else
            wr_enVETMAT when (onIVEC = '0' and onVETMAT = '1') else
            (others=>'0');                                      

-- it's a bypass between the RAM output and the inputs in VETMAT and HISTVECT
ramOutputVETMAT <= ramOutput;
ramOutputIVEC <= ramOutput;
   	
end SIMPLE;