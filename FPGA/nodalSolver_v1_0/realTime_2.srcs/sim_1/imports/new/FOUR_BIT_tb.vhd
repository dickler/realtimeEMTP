library IEEE; -- make IEEE library visible
use IEEE.STD_LOGIC_1164.all; -- open STD_LOGIC_1164 package
use IEEE.Numeric_Std.all;

entity TT is -- entity for test bed is
end TT; -- empty entity

architecture TT of TT is -- simple architecture statement....


signal clk : std_logic;
signal cmd : std_logic := '0';
signal counter: std_logic_vector(31 downto 0) := (others => '0');
signal v2 : std_logic_vector(31 downto 0);
--signal done : std_logic;
--signal cmdLC : std_logic := '0';
--signal doneLC : std_logic;

--signal rEq : STD_LOGIC_VECTOR(31 DOWNTO 0);
--signal vNode1 : STD_LOGIC_VECTOR(31 DOWNTO 0);
--signal vNode2 : STD_LOGIC_VECTOR(31 DOWNTO 0);

begin


main: entity work.main(behavioral) 
	port map (
	clk => clk,
	cmd => cmd,
	output => v2
);



--adder: entity work.vetmat(SIMPLE) 
--	port map ( 
--	clk => clk,
--	NW_CMD => cmd, 
--	NW_DONE => done
--);

--lcGroundedMemory: entity work.memoryAccess(SIMPLE) 
--	port map ( 
--	clk => clk,
--	externalCmd => cmdLC, 
--	externalDone => doneLC,
--	rEq => rEq,
--	vNode1 => vNode1,
--	vNode2 => vNode2
--);


clk_process : process
    begin
            clk <= '1';
            wait for 5ns;
            clk <= '0';
            wait for 5ns;
    end process;
    
    
 cmd_process : process
    begin
        counter <= std_logic_vector(unsigned(counter)+1);
        if unsigned(counter) = 1 then
            cmd <= '1';
        else 
            cmd <= '0';
        end if;
        wait for 10 ns;
    end process;
    
 
end TT;