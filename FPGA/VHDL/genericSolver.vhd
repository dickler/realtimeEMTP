library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.Numeric_Std.all;

entity networkSolver is
    Port ( clk : in STD_LOGIC;
           cmd : in STD_LOGIC;
           rst : in STD_LOGIC;
           GPIO_SW_C : in std_logic;
           GPIO_DIP : in std_logic_vector(7 downto 0);
           s_in : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
           led_o : out std_logic_vector(7 downto 0);
           outputA : out STD_LOGIC_VECTOR (15 downto 0);
           outputB : out STD_LOGIC_VECTOR (15 downto 0);
           outputC : out STD_LOGIC_VECTOR (15 downto 0);
           outputD : out STD_LOGIC_VECTOR (15 downto 0);
           sync : out std_logic);
end networkSolver;

architecture Behavioral of networkSolver is

-- count signal
signal clk_count : std_logic_vector(15 downto 0);
signal finishing : std_logic;

-- solver signals
signal ap_start : std_logic := '0';
--signal ap_rst : std_logic;
signal ap_done : std_logic := '0';
signal ap_return: std_logic_vector (127 downto 0);
signal result_a: std_logic_vector (15 downto 0);
signal result_b: std_logic_vector (15 downto 0);
signal result_c: std_logic_vector (15 downto 0);
signal result_d: std_logic_vector (15 downto 0);
signal s: STD_LOGIC_VECTOR(31 DOWNTO 0);

-- export signals 
signal output_a: std_logic_vector (15 downto 0) := "1000000000000000";
signal output_b: std_logic_vector (15 downto 0) := "1000000000000000";
signal output_c: std_logic_vector (15 downto 0) := "1000000000000000";
signal output_d: std_logic_vector (15 downto 0) := "1000000000000000";

-- unused solver signals 
signal ap_ready : std_logic; 
signal ap_idle : std_logic;

-- states variables
type stateType is (
    -- main states 
	start,
    solve,
	idle,
	error
);
signal state : stateType := start;
signal error_signal : std_logic := '0';

COMPONENT nodalSolver_0
  PORT (
    ap_clk : IN STD_LOGIC;
    ap_rst : IN STD_LOGIC;
    ap_start : IN STD_LOGIC;
    ap_done : OUT STD_LOGIC;
    ap_idle : OUT STD_LOGIC;
    ap_ready : OUT STD_LOGIC;
    ap_return : OUT STD_LOGIC_VECTOR(127 DOWNTO 0);
    s : IN STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

begin

counter_component : entity work.counter(behavioral)
    PORT MAP(
        clk => clk,
        cmd => cmd,
        rst => rst,
        count => clk_count,
        count_finishing => finishing
);

nodalSolver : nodalSolver_0
  PORT MAP (
    ap_clk => clk,
    ap_rst => rst,
    ap_start => ap_start,
    ap_done => ap_done,
    ap_idle => ap_idle,
    ap_ready => ap_ready,
    ap_return => ap_return,
    s => s
  );

process(clk) -- state machine for data flow control 
    begin
        if clk'event and clk='1' then
            if rst = '0' then
            case state is 
                when start =>
                    if cmd = '1' then
                        state <= solve;
                        ap_start <= '1';
                        s <= s_in;
                    else
                        ap_start <= '0'; 
                    end if;
                    
                when solve =>
                    ap_start <= '0';
                    if ap_done = '1' then
                        state <= idle;
                        result_a <= ap_return(15 downto 0);
                        result_b <= ap_return(47 downto 32);
                        result_c <= ap_return(79 downto 64);
                        result_d <= ap_return(111 downto 96);
                    end if;
                
                when idle =>
                    if finishing = '1' then
                        state <= solve;
                        s <= s_in;
                        output_a <= result_a;
                        output_b <= result_b;
                        output_c <= result_c;
                        output_d <= result_d;
                        ap_start <= '1';
                    end if;
                
                when error =>
                    error_signal <= '1';
            end case;
            
            else -- reset case
            state <= start;
            ap_start <= '0';
            output_a <= "1000000000000000";
            output_b <= "1000000000000000";
            output_c <= "1000000000000000";
            output_d <= "1000000000000000";
            end if;
            
            -- sync signal and led test
            sync  <= not GPIO_SW_C;
            led_o <= GPIO_DIP;
            
        end if;
    end process;

-- output signals
outputA <= output_a;
outputB <= output_b;
outputC <= output_c;
outputD <= output_d;

end Behavioral;
