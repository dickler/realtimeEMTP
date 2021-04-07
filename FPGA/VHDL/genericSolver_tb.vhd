library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.Numeric_Std.all;

entity genericSolver_tb is
--  Port ( );
end genericSolver_tb;

architecture Behavioral of genericSolver_tb is

signal clk : std_logic;
signal cmd : std_logic := '0';
signal rst : std_logic := '0';
signal sync : std_logic;
signal GPIO_SW_C : std_logic := '0';
signal s : std_logic_vector(31 downto 0) := (others => '0');

signal GPIO_DIP : std_logic_vector(7 downto 0) := (others => '0');
signal led_o : std_logic_vector(7 downto 0);

-- export signals 
signal outputA: std_logic_vector (15 downto 0);
signal outputB: std_logic_vector (15 downto 0);
signal outputC: std_logic_vector (15 downto 0);
signal outputD: std_logic_vector (15 downto 0);

begin

solver_component : entity work.networkSolver(behavioral)
    PORT MAP(
        clk => clk,
        cmd => cmd,
        rst => rst,
        GPIO_SW_C => GPIO_SW_C,
        GPIO_DIP => GPIO_DIP,
        s_in => s,
        led_o => led_o,
        outputA => outputA,
        outputB => outputB,
        outputC => outputC,
        outputD => outputD,
        sync => sync
);
           

clk_process : process
    begin
            clk <= '1';
            wait for 5ns;
            clk <= '0';
            wait for 5ns;
    end process;
    
s_process : process
    begin
        s(0) <= '0';
        wait for 50us;
        s(0) <= '1';
        wait for 50us;
    end process;

 cmd_process : process
    begin
        cmd <= '1';
        wait for 10 ns;
        cmd <= '0';
        wait for 1000 ns;
        GPIO_DIP <= "11010110";
        GPIO_SW_C <= '1';
        wait for 1000 ns;
        GPIO_SW_C <= '0';
        wait for 50090 ns;
        rst <= '1';
        wait for 10 ns;
        rst <= '0';
        wait for 50 ns;
        cmd <= '1';
        wait for 10 ns;
        cmd <= '0';
        wait for 500 ms;
    end process;
    
end Behavioral;
