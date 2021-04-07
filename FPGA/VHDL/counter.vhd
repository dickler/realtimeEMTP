library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.Numeric_Std.all;

entity counter is
    generic (N_CLK_PASS : integer := 100);
    Port ( clk : in STD_LOGIC;
           cmd : in STD_LOGIC;
           rst : in STD_LOGIC;
           count : out STD_LOGIC_VECTOR (15 downto 0);
           count_finishing : out STD_LOGIC);
end counter;

architecture Behavioral of counter is

signal i : std_logic_vector(15 downto 0) := (others=>'0');
signal finishing : std_logic := '0';

-- states variables
type stateType is (
    -- main states 
	start,
    counting
);
signal state : stateType := start;

begin

process(clk)
    begin
        if clk'event and clk='1' then
            if rst = '0' then
            case state is 
                when start =>
                    i <= (others => '0');
                    finishing <= '0';
                    if cmd = '1' then
                        state <= counting;
                    end if;
                
                when counting =>
                    if unsigned(i) = N_CLK_PASS - 1 then
                        i <= (others => '0');
                        finishing <= '0';
                    elsif unsigned(i) = N_CLK_PASS - 2 then
                        i <= std_logic_vector(unsigned(i) + 1);
                        finishing <= '1';
                    else
                        i <= std_logic_vector(unsigned(i) + 1);
                        finishing <= '0';
                    end if;
            end case;
            
            else -- reset case
                i <= (others => '0');
                finishing <= '0';
                state <= start;
            end if;
        end if;
    end process;
    
count <= i;
count_finishing <= finishing;
end Behavioral;