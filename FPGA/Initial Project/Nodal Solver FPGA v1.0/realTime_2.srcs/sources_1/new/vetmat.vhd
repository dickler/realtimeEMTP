-- matrix vector multiplication module. multiplicator should have 1 cycle delay and accumulator 2 cycle delay. 
-- matrx and vector are catched from a rom memory, loaded from a coe file.
-- result are stored in ram memory signal called ramOutput
-- after a pulse in cmd signal, the multiplication begins
-- as it finishes, done signal pulses.
-- it is neede at least one clock signal after done pulses to the last value be assigned in ram memory
-- improvements: change vector storage to ram memory, in order to seem more like a dynamic network solver
-- doubts: how to share same ram memory between modules? just instantiating the same ram IP core?
--


library IEEE; -- make IEEE library visible
use IEEE.STD_LOGIC_1164.all; -- open STD_LOGIC_1164 package
use IEEE.Numeric_Std.all;

entity VETMAT is
    generic (matrixOrder : integer := 4);
    port (  -- clock
            clk : in std_logic;
            
            -- begin multiplying order
            NW_CMD : in STD_LOGIC;
            
            -- vector V input (from vOnePortRam) 
            ramOutputV : in STD_LOGIC_VECTOR (31 downto 0);
            
            -- vector I input (from hDualPortRam) it's used in vector matrix multiplying
            ramOutputH : in STD_LOGIC_VECTOR (31 downto 0);    
            
            -- signal to inform VETMAT is multiplying
            onVETMAT : out STD_LOGIC;                   
            
            -- signals to arbiterV
            NW_DONE : out STD_LOGIC;
            wr_enV : out STD_LOGIC_VECTOR (0 downto 0);
            ramAddressV : out STD_LOGIC_VECTOR (3 downto 0);
            ramInputV : out STD_LOGIC_VECTOR (31 downto 0);
            
            -- signal to access vector I in the RAM memory hDualPortRAM
            ramAddressBH : out STD_LOGIC_VECTOR (3 downto 0);
            ramAddressAH : out STD_LOGIC_VECTOR (3 downto 0);
            
            -- signals to write in vRamOnePort
            wr_enH : out STD_LOGIC_VECTOR (0 downto 0);
            ramInputH : out STD_LOGIC_VECTOR (31 downto 0)
                    
);
end VETMAT; 

architecture SIMPLE of VETMAT is 

constant matrixOrder2 : integer := matrixorder*matrixorder; --square of matrix order

signal cmd : std_logic := '0';
signal done : std_logic := '0';
-- signal delayFinishState : std_logic := '0';
signal delayFinishState : std_logic_vector(2 downto 0) := "000";

--accumulator's signals
signal accumLast : std_logic_vector(0 downto 0) := "0";
signal accumLastReg : std_logic_vector(0 downto 0);
signal accumValidA : std_logic := '0';
signal accumValidAreg : std_logic := '0';
signal accumOutput : STD_LOGIC_VECTOR(31 DOWNTO 0);

--rom's signals
signal addressA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
--signal addressB : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal romContentA : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal romContentAreg : STD_LOGIC_VECTOR(31 DOWNTO 0);
--signal romContentB : STD_LOGIC_VECTOR(31 DOWNTO 0);

--ram's signals
signal ramAddressV_i : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal wr_enV_i : std_logic_vector(0 downto 0) := "0";

--ram's signals
signal ramAddressBH_i : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal ramAddressAH_i : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal wr_enH_i : std_logic_vector(0 downto 0) := "0";

-- I_ROM signals
signal iRomAddress : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal iRomAddressReg : STD_LOGIC_VECTOR(3 DOWNTO 0);

--fsm signals

	type stateType is (
		idle,
		multiplying,
		finished
		);

	signal state   : stateType := idle;
	signal counter : integer range 0 to 3 := 0;
    signal disableMULT : STD_LOGIC := '0';
    signal disableMULTREG : STD_LOGIC;
    
COMPONENT dist_mem_gen_0
  PORT (
    a : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    spo : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;


COMPONENT c_shift_ram_0
  PORT (
    D : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_4
  PORT (
    D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_1
  PORT (
    D : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_11
  PORT (
    D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
  );
END COMPONENT;

COMPONENT blk_mem_gen_5 -- ROM to reset RAM memory H to default
  PORT (
    clka : IN STD_LOGIC;
    addra : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

begin 

distributedRam : dist_mem_gen_0
  PORT MAP (
    a => addressA,
    spo => romContentAreg
  );
  
I_ROM : blk_mem_gen_5
  PORT MAP (
    clka => clk,
    addra => iRomAddress,
    douta => ramInputH
  );
  
shift_register0 : c_shift_ram_0
  PORT MAP (
    D => accumLastReg,
    CLK => CLK,
    Q => wr_enV_i
  );
  
shift_register1 : c_shift_ram_1 -- one cycle shift register
    PORT MAP (
      D(0) => accumValidA,
      CLK => CLK,
      Q(0) => accumValidAreg
    );
    
shift_register2 : c_shift_ram_1 -- one cycle shift register
        PORT MAP (
          D => accumLast,
          CLK => CLK,
          Q => accumLastreg
        );
        
shift_register3 : c_shift_ram_1 -- one cycle shift register
                PORT MAP (
                  D(0) => disableMULT,
                  CLK => CLK,
                  Q(0) => disableMULTREG
                );          
        
shift_register4 : c_shift_ram_4 -- one cycle shift register
PORT MAP (
  D => romContentAreg,
  CLK => CLK,
  Q => romContentA
);

registerRomAddress : c_shift_ram_11 -- three cycle shift register
    PORT MAP (
      D => iRomAddress,
      CLK => CLK,
      Q => iRomAddressReg
    );
	
adder: entity work.MAC(SIMPLE) 
      port map ( 
      clk => clk,
      i_a => romContentA, 
      i_b => ramOutputH, 
      i_reset => accumLastReg(0),
      accumValidA => accumValidAreg,
      disable => disableMULTREG,
      output => accumOutput
      );
	
process(clk) -- upgrade possibility: remove addressB assignation in order to prevent mealy machine, moving it to another process

    begin
        if clk'event and clk='1' then
                case state is
				
				    -- it's a hold state 
                    when idle =>  -- all the outputs are zero
                        delayFinishState <= "000";
                        onVETMAT <= '0';
						accumValidA <= '0';
					    disableMULT <= '1';						
						done <= '0';
						addressA <= (others => '0');
						ramAddressBH_i <= (others => '0');
						accumLast <= "0";
								
                        if cmd = '0' then -- state change when cmd != '0'
                            state <= idle;
                        else
                            state <= multiplying;
                        end if;						
						
					-- it's the matrix X vector multiplicator state
					when multiplying =>
					    onVETMAT <= '1'; 
						accumValidA <= '1';
						disableMULT <= '0';						
					    -- done <= '0';
						addressA <= std_logic_vector(unsigned(addressA)+1);
						
						if (unsigned(ramAddressBH_i)+1) = matrixorder then --last vector element, reset
							ramAddressBH_i <= (others => '0');					
							accumLast <= "1";
							
							if (unsigned(addressA)) = matrixorder2 - 1 then -- if all the conductance is scrolled, pass to next state
								state <= finished;
								wr_enH_i <= "1";
								iRomAddress <= std_logic_vector(unsigned(iRomAddress)+1);
							else
								state <= multiplying;
							end if;	
												
						else
							ramAddressBH_i <= std_logic_vector(unsigned(ramAddressBH_i)+1);
							accumLast <= "0";
							state <= multiplying;							
						end if;
					
					-- final state
                    when finished => -- it exists to set all the outputs to zero again and to wait one more clock to save the output vector V in the RAM memory
                            accumValidA <= '1';
                            disableMULT <= '1';
                            -- done <= '1';
                            addressA <= (others => '0');
                            ramAddressBH_i <= (others => '0');
                            accumLast <= "0";
                            
                            iRomAddress <= std_logic_vector(unsigned(iRomAddress)+1);
                            
                            delayFinishState <= std_logic_vector(unsigned(delayFinishState)+1);
                            if unsigned(ramAddressAH_i)+1 = matrixorder then
						          state <= idle;
						          delayFinishState <= "000";
						          wr_enH_i <= "0";
						          iRomAddress <= (others => '0');
						          done <= '1';
                            end if;
--                            if done <= '0' then --need a delay in onVETMAT                  			    
--						        state <= finished;
--						        else
--						        if delayFinishState <= '0' then
--						          state <= finished;
--						          delayFinishState <= '1';
--						        else
--						          state <= idle;
--						        end if;
--						    end if;
						    
                end case;
                
        end if;
		
    end process;
    
ramAddressProcess: process(clk) -- process to access the next address in the RAM memory
    begin
        if clk'event and clk='1' then
            if wr_enV_i = "1" then                
                if (unsigned(ramAddressV_i)+1) = matrixorder then 
                    ramAddressV_i <= (others => '0');
                else 
                    ramAddressV_i <=  std_logic_vector(unsigned(ramAddressV_i)+1);
                end if;
            else
            ramAddressV_i <= ramAddressV_i; -- is this a good practice? how to avoid latche in such a situation?
            end if;
        end if;       
end process;
    
	NW_done <= done;
	cmd <= NW_cmd;
	wr_enV <= wr_enV_i;
	ramAddressV <= ramAddressV_i;
	ramInputV <= accumOutput;
	
	-- I_rom access
	ramAddressAH_i <= iRomAddressReg;
	
	-- hRAM access control
	ramAddressBH <= ramAddressBH_i;
	ramAddressAH <= ramAddressAH_i;
	wr_enH <= wr_enH_i ;
	
end SIMPLE;