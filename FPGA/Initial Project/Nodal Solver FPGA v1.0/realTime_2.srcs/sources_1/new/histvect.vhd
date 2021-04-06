
library IEEE; -- make IEEE library visible
use IEEE.STD_LOGIC_1164.all; -- open STD_LOGIC_1164 package
use IEEE.Numeric_Std.all;

entity histvect is
    generic (elementsNumber : integer := 2); -- number of elements of COE file. one of this type will be needed for capacitor, inductor, and current sources
    port (  -- clock
            clk : in std_logic;
            
            -- vector H (it's not being used for while)
            ramOutputH : in STD_LOGIC_VECTOR(31 DOWNTO 0);
            
            -- vector V
            ramOutputV : in STD_LOGIC_VECTOR(31 DOWNTO 0);
            
            -- begin to store elements
            externalCmd : in STD_LOGIC; 
            
            -- end of calculation                 
            externalDone : out STD_LOGIC;
            
            -- flag for on state
            onIVEC : out std_logic; 
            
            -- vRAM write enable (it's not being used for while)
            wr_enV : out STD_LOGIC_VECTOR (0 downto 0) := "0";
            
            -- vRAM request adrress
            ramAddressV : out STD_LOGIC_VECTOR (3 downto 0);
            
            -- vRAM write data (it's not being used for while)
            ramInputV : out STD_LOGIC_VECTOR(31 DOWNTO 0);
            
            -- hRAM write enable (it's not being used for while)
            wr_enH : out STD_LOGIC_VECTOR (0 downto 0);
            
            -- hRAM request adrressA
            ramAddressAH : out STD_LOGIC_VECTOR (3 downto 0);
            
            -- hRAM request adrressB
            ramAddressBH : out STD_LOGIC_VECTOR (3 downto 0);
            
            -- hRAM write data
            ramInputH : out STD_LOGIC_VECTOR(31 DOWNTO 0)
                        
);
end histvect; 

architecture SIMPLE of histvect is 

	type stateType is (
		standBy,
		start,
		finish
		);

	signal state   : stateType := standBy;
	signal first : std_logic := '1';
	
    signal cmd : std_logic := '0';
    signal done : std_logic := '0';	
    
    signal vRamAccessControl : std_logic := '0';
    -- signal demuxAddress : std_logic_vector(2 downto 0) := "000";
    
--ram's signals
    signal P : std_logic;
    signal PReg : std_logic;
    
    signal nodeKisZero : std_logic;
    signal nodeMisZero : std_logic;
    signal nodeKRegIsZero : std_logic;
    signal nodeMRegIsZero : std_logic;
    signal nodeRegIsZero : std_logic;
    
    signal nodeK : std_logic_vector(3 downto 0);
    signal nodeKreg : std_logic_vector(3 downto 0);
    signal nodeKreg2 : std_logic_vector(3 downto 0);
    
    signal nodeM : std_logic_vector(3 downto 0);
    signal nodeMreg : std_logic_vector(3 downto 0);
    signal conductance : std_logic_vector(31 downto 0);
    signal conductanceReg : std_logic_vector(31 downto 0);

    signal IhOld : std_logic_vector(31 downto 0);
    signal IhOldNew : std_logic_vector(31 downto 0);
    signal IhOldNew2 : std_logic_vector(31 downto 0);
    signal IhNew : std_logic_vector(31 downto 0);
    
    signal ramAddressH : std_logic_vector(3 downto 0) := (others=> '0');
    signal ramInputH_i : std_logic_vector(31 downto 0) := (others=> '0');
    --after XOR operation
--
    signal vK : std_logic_vector(31 downto 0) := (others=> '0');
    signal vM : std_logic_vector(31 downto 0) := (others=> '0');
    signal vKM : std_logic_vector(31 downto 0) := (others=> '0');
    signal ramOutputV_i : std_logic_vector(31 downto 0);
    signal voltageV : std_logic_vector(31 downto 0);
    signal ramAddressV_i : std_logic_vector(3 downto 0);
    signal mult1 : std_logic_vector(31 downto 0); --multiplication result
    signal mult2 : std_logic_vector(31 downto 0); --multiplication result after xor operation for correct signal
    signal sum1 : std_logic_vector(31 downto 0); --summation result
    signal sum2 : std_logic_vector(31 downto 0); -- opposite summation result
    
    signal counterFinish : std_logic_vector(11 downto 0) := (others => '0');
    -- signal counter : std_logic_vector(11 downto 0) := (others => '0');
    signal counterNodes : std_logic_vector(2 downto 0) := "000";
    
    signal  m_axis_result_tvalidSub : std_logic;  
    signal  m_axis_result_tvalidMult : std_logic;
    signal  m_axis_result_tvalidSum : std_logic;
    signal  m_axis_result_tvalidSumH : std_logic;
    
    -- LC components ROM signals
    signal ramAddress : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
    signal NodesRamAddress : STD_LOGIC_VECTOR(2 DOWNTO 0) := "000";
    signal NodesRamAddressreg : STD_LOGIC_VECTOR(2 DOWNTO 0) := "000";
    signal NodesValue : STD_LOGIC_VECTOR(64 DOWNTO 0) := (others => '0');
    signal histWr_en : std_logic_vector(0 downto 0) := "0";
    signal histWr_enReg : std_logic_vector(0 downto 0) := "0";
    
    --historical current for each LC RAM memory signals
    signal iwr_en : std_logic_vector(0 downto 0) := "0";
    signal iwr_enReg : std_logic_vector(0 downto 0) := "0";
    signal iramAddress : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
    signal iRamInput : std_logic_vector(31 downto 0); 
    signal iRamOutput : std_logic_vector(31 downto 0); 
    
COMPONENT blk_mem_gen_2 -- nodes ROM
  PORT (
    clka : IN STD_LOGIC;
    addra : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(64 DOWNTO 0)
  );
END COMPONENT;

COMPONENT blk_mem_gen_3
  PORT (
    clka : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT floating_point_3 --1cycle subtrator
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

COMPONENT floating_point_4 --1cycle multiplier
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

COMPONENT floating_point_5 --1cycle sum
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

COMPONENT floating_point_2 --1cycle sum2
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

COMPONENT c_shift_ram_2 --3 cycle 32 bit register
  PORT (
    D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_3 --3 cycle 4 bit register
  PORT (
    D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_5 --4 cycle 4 bit register
  PORT (
    D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_6 --3 cycle 3 bit register --increase number of bits
  PORT (
    D : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(2 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_7 --5 cycle 4 bit register
  PORT (
    D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_8 --3 cycle 1 bit register
  PORT (
    D : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_9 --6 cycle 1 bit register
  PORT (
    D : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_10 --7 cycle 1 bit register
  PORT (
    D : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
  );
END COMPONENT;

begin 

subtractor : floating_point_3 -- subtractor
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => '1',
    s_axis_a_tdata => vK,
    s_axis_b_tvalid => '1',
    s_axis_b_tdata => vM,
    m_axis_result_tvalid => m_axis_result_tvalidSub,
    m_axis_result_tdata => vKM
  );
  
  multiplier : floating_point_4 -- multiplier
    PORT MAP (
      aclk => clk,
      s_axis_a_tvalid => '1',
      s_axis_a_tdata => vKM,
      s_axis_b_tvalid => '1',
      s_axis_b_tdata => conductanceReg,
      m_axis_result_tvalid => m_axis_result_tvalidMult,
      m_axis_result_tdata => Mult1
    );

sum : floating_point_5 -- sum
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => '1',
    -- s_axis_a_tdata => IhOld,
    s_axis_a_tdata => IhOldNew2,
    s_axis_b_tvalid => '1',
    -- s_axis_b_tdata => mult1,
    s_axis_b_tdata => mult2,
    m_axis_result_tvalid => m_axis_result_tvalidSum,
    m_axis_result_tdata => sum1
  );

sumH : floating_point_2 -- sumH
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => '1',
    s_axis_a_tdata => IhNew,
    s_axis_b_tvalid => '1',
    s_axis_b_tdata => IhOld,
    m_axis_result_tvalid => m_axis_result_tvalidSumH,
    m_axis_result_tdata => RamInputH_i
  );
  
nodesRam : blk_mem_gen_2 -- it's actually a ROM
  PORT MAP (
    clka => clk,
    addra => NodesRamAddress,
    douta => NodesValue
  );
  
iRam : blk_mem_gen_3
    PORT MAP (
      clka => CLK,
      wea => iWr_enReg,
      addra => iRamAddress,
      dina => iRamInput,
      douta => iRamOutput
    ); 

conductanceRegister : c_shift_ram_2
  PORT MAP (
    D => conductance,
    CLK => CLK,
    Q => conductanceReg
  );
  
nodeKregister : c_shift_ram_3 --nodek register for access histvect ram 3 cycle 4 bit
PORT MAP (
  D => nodeK,
  CLK => CLK,
  Q => nodeKreg
);

nodeKregister2 : c_shift_ram_5  --nodek register for parse address in  histvect - 4cycle 4bit
PORT MAP (
  D => nodeK,
  CLK => CLK,
  Q => nodeKreg2
);

nodesRamAdressRegister : c_shift_ram_6  --nodek register for parse address in  histvect - 4cycle 4bit
PORT MAP (
  D => NodesRamAddress,
  CLK => CLK,
  Q => NodesRamAddressReg
);

nodeMregister : c_shift_ram_7  --nodem register for parse address in  histvect - 5cycle 4bit
PORT MAP (
  D => nodeM,
  CLK => CLK,
  Q => nodeMreg
);

Pregister : c_shift_ram_8  --p register for synchronizing P in mult1 and Ihold
PORT MAP (
  D(0) => P,
  CLK => CLK,
  Q(0) => PReg
);

iWregister : c_shift_ram_9  -- register to write in the iRam
PORT MAP (
  D => iwr_en,
  CLK => CLK,
  Q => iwr_enReg
);

histWregister : c_shift_ram_10  -- register to write in the hRam
PORT MAP (
  D => histWr_en,
  CLK => CLK,
  Q => histWr_enReg
);

--direct instantiation

demux: entity work.demux(behaviour) 
	port map ( 
--	clk => clk,
	--data => ramOutputV_i, 
	data => voltageV,
	address => counterNodes,
	vk => vk,
	vm => vm
);

process(clk) -- upgrade possibility: remove addressB assignation in order to prevent mealy machine, moving it to another process 
      begin
          if clk'event and clk='1' then
                  case state is
                  
                      when standBy =>
                      onIVEC <= '0';
                      done <= '0';
                      counterFinish <= (others => '0');           
                                   
                          if cmd = '0' then 
                              state <= standBy;
                          else
                              state <= start;
                              onIVEC <= '1';
                              vRamAccessControl <= '1';
                          end if;                        
                          
                      when start => -- replacing all "011" and 3 for "010" and 2, we can reduce the number of clocks to three
                            -- sum2 <= not sum1(31 downto 31) & sum1(30 downto 0); -- por quê???
                            -- counter <= std_logic_vector(unsigned(counter) + 1);
                            -- vRamAccessControl <= '0';
                            -- counter <= not counter;
                            
                            counterNodes <= std_logic_vector(unsigned(counterNodes) + 1);
                            -- if counterNodes = "100" then
                            if counterNodes = "011" then
                                counterNodes <= "000";     
                                -- vRamAccessControl <= '1';   
                                NodesRamAddress <=  std_logic_vector(unsigned(NodesRamAddress)+1);  
                                --counter <= (others => '0');                                                  
                            end if; 
                            
                                
                            -- it's a condition to control the data that goes to subtractor 
                            -- if unsigned(counterNodes) = 2 then
                            if unsigned(counterNodes) = 1 then
                                vRamAccessControl <= '0';
                            end if;
                            
                            -- if unsigned(counterNodes) = 0 then
                            if unsigned(counterNodes) = 3 then
                                vRamAccessControl <= '1';
                            end if;
                             
                           -- condition to state change
                            if ((unsigned(NodesRamAddress)) = elementsNumber - 1) and (counterNodes = "011") then 
                                NodesRamAddress <= (others => '0');
                                state <= finish;                              
                            else
                                state <= start;
                            end if;
                     
                      when finish =>
                            -- sum2 <= not sum1(31 downto 31) & sum1(30 downto 0);
                            -- vRamAccessControl <= not vRamAccessControl;
                            vRamAccessControl <= '0';
                            counterFinish <= std_logic_vector(unsigned(counterFinish) + 1);
                            counterNodes <= std_logic_vector(unsigned(counterNodes) + 1);
                            if unsigned(counterFinish) = 4 then 
                                state <= standBy;
                                done <= '1';
                                counterNodes <= (others => '0');
                                onIVEC <= '0';
                            end if;

                  end case;
                  
          end if;
          
      end process;
    
    -- define sum2
    sum2 <= not sum1(31 downto 31) & sum1(30 downto 0);
    
    -- accessing data from ROM L and C
    nodeK <= NodesValue(51 downto 48);
    nodeM <= NodesValue(35 downto 32);
    conductance <= NodesValue(31 downto 0);
    P <= NodesValue(64);    
    
    --addressing for retrieving vK and vM 
    nodeKIsZero <= '1' when unsigned(nodeK) = 0 else '0';
    nodeMIsZero <= '1' when unsigned(nodeM) = 0 else '0';
    voltageV <= (others => '0') when ((nodeKIsZero = '1' and unsigned(counterNodes) = 2) or
                                     (nodeMIsZero = '1' and unsigned(counterNodes) = 3))
                                else ramOutputV_i;
    ramAddressV_i <= nodeK when vRamAccessControl = '1' else nodeM; 
    ramAddressV <= std_logic_vector(unsigned(ramAddressV_i) - 1);
    
    -- addressing for vector Hist
    ramAddressH <= std_logic_vector(unsigned(nodeKreg)-1) 
                    when unsigned(counterNodes) = 1 or unsigned(counterNodes) = 3 
                    else std_logic_vector(unsigned(nodeMreg)-1);
    ramAddressAH <= ramAddressH;
    ramAddressBH <= ramAddressH;
    
    -- writing for vector Hist
    nodeKRegIsZero <= '1' when unsigned(nodeK) = 0 else '0';
    nodeMRegIsZero <= '1' when unsigned(nodeM) = 0 else '0';
    nodeRegIsZero <= nodeKRegIsZero when unsigned(counterNodes) = 3 else
                     nodeMRegIsZero when unsigned(counterNodes) = 0 or unsigned(counterNodes) = 4
                     else '0';
    wr_enH(0) <= histWr_enReg(0) and (not nodeRegIsZero);
    
    -- hRam write logic
    histWr_en(0) <= '1' when vRamAccessControl = '1' and state = start else '0';
    IhOld <= ramOutputH;
    IhNew <= sum1 when unsigned(counterNodes) = 1 or unsigned(counterNodes) = 3 else sum2;
    ramInputH <= ramInputH_i;
       
    -- ramInputH <= sum1 when vRamAccessControl = '0' else sum2;
    -- iRam write logic
    iRamInput <= sum1;
    iWr_en(0) <= '1' when unsigned(counterNodes) = "000" and (state = start) else '0'; --writing I values
    
    -- others important signals' assignment
    ramOutputV_i <= ramOutputV;    
	mult2 <= (mult1(31) xor PReg) & mult1(30 downto 0);
    IhOldNew2 <= (IhOldNew(31) xor PReg) & IhOldNew(30 downto 0);
	iRamAddress <= '0'&NodesRamAddressReg;
	IhOldNew <= iRamOutput;
	
	externalDone <= done;
    cmd <= externalCmd;
    	
end SIMPLE;