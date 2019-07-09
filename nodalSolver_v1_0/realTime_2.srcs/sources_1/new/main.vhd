library IEEE; 
use IEEE.STD_LOGIC_1164.all;
use IEEE.Numeric_Std.all;

entity main is
    generic (integrationPass : integer := 100); -- it refers to the number of clocks in a integration pass
    port (
            clk : in std_logic; --clock source
            cmd : in std_logic; -- command for start simulation
            output : out std_logic_vector (31 downto 0)); -- output signal
            --cmd_NW : in std_logic; -- command for vetmat module
            --cmd_LC : in std_logic -- command for histvect module
end main; 

architecture behavioral of main is 

signal output_i : std_logic_vector (31 downto 0);
signal output_aux : std_logic_vector (31 downto 0);

signal done_NW : std_logic;
signal done_LC : std_logic;
signal onIVEC : std_logic;
signal onVETMAT : std_logic;
signal cmd_NW : std_logic := '0'; -- command for vetmat module
signal cmd_LC : std_logic := '0'; -- command for histvect module
signal intCounter : std_logic_vector(6 downto 0) := (others => '0'); -- it has 7 bits because of the integration pass

--ram's signals

--module VETMAT to arbiter V
signal VETMATtoARBvRamAddress : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal VETMATtoARBvRamInput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal VETMATtoARBvRamOutput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal VETMATtoARBvWr_en : std_logic_vector(0 downto 0) := "0";

--module VETMAT to arbiter H
signal VETMATtoARBhRamAddressA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal VETMATtoARBhRamAddressB : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal VETMATtoARBhRamInput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal VETMATtoARBhRamOutput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal VETMATtoARBhWr_en : std_logic_vector(0 downto 0) := "0";

--module HISTVECT to arbiter V
signal HISTVECTtoARBvRamAddress : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal HISTVECTtoARBvRamInput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal HISTVECTtoARBvRamOutput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal HISTVECTtoARBvWr_en : std_logic_vector(0 downto 0) := "0";

--module HISTVECT to arbiter H
signal HISTVECTtoARBhRamAddressA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal HISTVECTtoARBhRamAddressB : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal HISTVECTtoARBhRamInput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal HISTVECTtoARBhRamOutput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal HISTVECTtoARBhWr_en : std_logic_vector(0 downto 0) := "0";

-- vRAM to arbiterV
signal voltVectRamAddress : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal voltVectRamInput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal voltVectRamOutput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal voltVectWr_en : std_logic_vector(0 downto 0) := "0";

-- hRAM to arbiterH
signal histVectRamAddressA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal histVectRamAddressB : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
signal histVectRamInput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal histVectRamOutput : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal histVectWr_en : std_logic_vector(0 downto 0) := "0";
--

-- states variables
type stateType is (
	start,
	Ivector,
	matrixMul,
	idle,
	error
);
signal state   : stateType := start;
---- LC memory signals

--signal rEq : STD_LOGIC_VECTOR(31 DOWNTO 0);
--signal vNode1 : STD_LOGIC_VECTOR(31 DOWNTO 0);
--signal vNode2 : STD_LOGIC_VECTOR(31 DOWNTO 0);

COMPONENT blk_mem_gen_0
  PORT (
    clka : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT blk_mem_gen_1
  PORT (
    clka : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    clkb : IN STD_LOGIC;
    addrb : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;


begin 


vOnePortRam : blk_mem_gen_0
  PORT MAP (
    clka => CLK,
    wea => voltVectWr_en,
    addra => voltVectRamAddress,
    dina => voltVectRamInput,
    douta => voltVectRamOutput
  ); 
  
hDualPortRam : blk_mem_gen_1
    PORT MAP (
      clka => clk,
      wea => histVectWr_en,
      addra => histVectRamAddressA,
      dina => histVectRamInput,
      clkb => clk,
      addrb => histVectRamAddressB,
      doutb => histVectRamOutput
    );  
  
  
 vetmat : entity work.vetmat(SIMPLE) 
           port map ( 
           clk => clk,
           NW_CMD => cmd_NW,
           onVETMAT => onVETMAT, 
           NW_DONE => done_NW, 
           wr_enV => VETMATtoARBvWr_en,
           ramAddressV => VETMATtoARBvRamAddress,
           ramInputV => VETMATtoARBvRamInput,
           ramOutputV => VETMATtoARBvRamOutput,
           wr_enH => VETMATtoARBhWr_en,
           ramAddressAH => VETMATtoARBhRamAddressA,
           ramAddressBH => VETMATtoARBhRamAddressB,
           ramInputH => VETMATtoARBhRamInput,
           ramOutputH => VETMATtoARBhRamOutput       
           );
        
arbiter : entity work.arbiterV(SIMPLE)
        port map (  
        clk => CLK,
        onIVEC => onIVEC,
        onVETMAT => onVETMAT,      
        wr_en      => voltVectWr_en,
        ramAddress => voltVectRamAddress,  
        ramInput => voltVectRamInput,
        ramOutput => voltVectRamOutput,              
        wr_enVETMAT => VETMATtoARBvWr_en,
        ramAddressVETMAT => VETMATtoARBvRamAddress, 
        ramInputVETMAT => VETMATtoARBvRamInput,
        ramOutputVETMAT => VETMATtoARBvRamOutput,                
        wr_enIVEC => HISTVECTtoARBvWr_en,
        ramAddressIVEC => HISTVECTtoARBvRamAddress,
        ramInputIVEC => HISTVECTtoARBvRamInput,
        ramOutputIVEC => HISTVECTtoARBvRamOutput
);

arbiterHIST : entity work.arbiterHIST(SIMPLE)
        port map (  
        clk => CLK,
        onIVEC => onIVEC,
        onVETMAT => onVETMAT,      
        wr_en      => histVectWr_en,
        ramAddressA => histVectRamAddressA,
        ramAddressB => histVectRamAddressB,  
        ramInput => histVectRamInput, 
        ramOutput => histVectRamOutput,             
        wr_enVETMAT => VETMATtoARBhWr_en,
        ramAddressAVETMAT => VETMATtoARBhRamAddressA,
        ramAddressBVETMAT => VETMATtoARBhRamAddressB, 
        ramInputVETMAT => VETMATtoARBhRamInput,
        ramOutputVETMAT => VETMATtoARBhRamOutput,               
        wr_enIVEC => HISTVECTtoARBhWr_en,
        ramAddressAIVEC => HISTVECTtoARBhRamAddressA,
        ramAddressBIVEC => HISTVECTtoARBhRamAddressB,
        ramInputIVEC => HISTVECTtoARBhRamInput,
        ramOutputIVEC => HISTVECTtoARBhRamOutput
);                             
                

histvect: entity work.histvect(SIMPLE) 
        port map ( 
        clk => clk,
        externalCmd => cmd_LC, 
        externalDone => done_LC,
        onIVEC => onIVEC,
        wr_enV => HISTVECTtoARBvWr_en,
        ramAddressV => HISTVECTtoARBvRamAddress,
        ramInputV => HISTVECTtoARBvRamInput,
        ramOutputV => HISTVECTtoARBvRamOutput,
        wr_enH => HISTVECTtoARBhWr_en,
        ramAddressAH => HISTVECTtoARBhRamAddressA,
        ramAddressBH => HISTVECTtoARBhRamAddressB,
        ramInputH => HISTVECTtoARBhRamInput,
        ramOutputH => HISTVECTtoARBhRamOutput   
        );
        
process(clk)
    begin
        if clk'event and clk='1' then
                intCounter <= std_logic_vector(unsigned(intCounter)+1);
                case state is
				    -- it's a hold state 
                    when start =>
                        if cmd = '1' then
                            state <= Ivector;
                            cmd_LC <= '1';
                        end if;
                        
                    when Ivector => -- calculate and sum the I vector
                        --intCounter <= std_logic_vector(unsigned(intCounter)+1);
                        cmd_LC <= '0';
                        if done_LC = '1' then
                            state <= matrixMul;
                            cmd_NW <= '1';
                        end if;
                        
                    when matrixMul =>
                        --intCounter <= std_logic_vector(unsigned(intCounter)+1);
                        cmd_NW <= '0';
                        if done_NW ='1' then
                            state <= idle;
                        end if;
                        
                        if voltVectWr_en = "1" and unsigned(VETMATtoARBvRamAddress) = 2 then
                            output_aux <= VETMATtoARBvRamInput;
                        end if;
                    
                    when idle =>
                        --intCounter <= std_logic_vector(unsigned(intCounter)+1);
                        if unsigned(intCounter)+1 = integrationPass
                           or unsigned(intCounter) = integrationPass then
                                state <= Ivector;
                                cmd_LC <= '1';
                                intCounter <= (others => '0');
                                output_i <= output_aux;
                        elsif unsigned(intCounter) > integrationPass then
                                state <= error;
                        end if;
                        
                    when error =>
                        state <= error;
                end case;
            end if;
        end process;
        
output <= output_i;

end behavioral;