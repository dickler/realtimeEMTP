----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10.07.2019 16:23:52
-- Design Name: 
-- Module Name: sinVoltageGen - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.Numeric_Std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity sinVoltageGen is
    generic (sourcesNumber : integer := 1;
             nodesNumber : integer := 2);
    Port ( clk : in STD_LOGIC;
           cmd : in STD_LOGIC;
           --ramOutputBH : in STD_LOGIC_VECTOR(31 DOWNTO 0);
           
           onSinGen : out STD_LOGIC;
           doneSinGen : out STD_LOGIC;
           ramInputBH : out STD_LOGIC_VECTOR(31 DOWNTO 0);
           ramAddressBH : out STD_LOGIC_VECTOR(3 DOWNTO 0);
           wr_enBH : out STD_LOGIC_VECTOR(0 DOWNTO 0));
end sinVoltageGen;

architecture Behavioral of sinVoltageGen is

type stateType is (
		hold,
		generating,
		finishing
		);
signal state : stateType := hold;
signal valid : std_logic := '1';
signal counterFinish : std_logic_vector(2 downto 0) := "000";

-- signals from tRAM
signal integrationPass : std_logic_vector(31 downto 0);
signal integrationPassReg : std_logic_vector(31 downto 0); -- used to save in tRAM
signal phaseOld : std_logic_vector(31 downto 0);
signal amplitude : std_logic_vector(31 downto 0);
signal amplitudeReg1 : std_logic_vector(31 downto 0); -- used to save in tRAM
signal amplitudeReg2 : std_logic_vector(31 downto 0); -- used in multiplication
signal offSet : std_logic_vector(31 downto 0);
signal offSetReg1 : std_logic_vector(31 downto 0); -- used to save in tRAM
signal offSetReg2 : std_logic_vector(31 downto 0); -- used in sum2

-- conversion signals
signal phaseFloat : std_logic_vector(31 downto 0) := (others=>'0');
signal phaseFixed : std_logic_vector(31 downto 0);
signal resultFixed : std_logic_vector(63 downto 0);
signal sinFixed : std_logic_vector(31 downto 0);
signal sinFloat : std_logic_vector(31 downto 0);

-- multplier signal
signal mult : std_logic_vector(31 downto 0);

-- sum1 signal
signal sum1 : std_logic_vector(31 downto 0);

-- sumPi signal
signal sum2 : std_logic_vector(31 downto 0);

-- comparer signals
signal comparation : std_logic_vector(7 downto 0);
signal pi : std_logic_vector(31 downto 0) := "01000000010010010000111111011010";
signal negativePi : std_logic_vector(31 downto 0) := "11000000010010010000111111011010";

-- validating operations
signal cordicValid : std_logic;
signal FixToFltValid : std_logic;
signal FltToFixValid : std_logic;
signal sumValid : std_logic;
signal sum2Valid : std_logic;
signal sumOffSetValid : std_logic;
signal multValid : std_logic;
signal comparerValid : std_logic;

-- tRAM signals
signal tRamW_enA : STD_LOGIC_VECTOR(0 DOWNTO 0);
signal tRamAddressA : std_logic_vector(3 downto 0);
signal tRamInputA : std_logic_vector(127 downto 0);

signal tRamAddressB : std_logic_vector(3 downto 0);
signal tRamAddressBreg : std_logic_vector(3 downto 0);
signal tRamOutputB : std_logic_vector(127 downto 0);

-- generic write enable to write in tRAM and hRAM
signal wr_en : STD_LOGIC_VECTOR(0 DOWNTO 0) := "0";

-- 




COMPONENT blk_mem_gen_4 -- tRAM
  PORT (
    clka : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(127 DOWNTO 0);
    clkb : IN STD_LOGIC;
    addrb : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(127 DOWNTO 0)
  );
END COMPONENT;

COMPONENT cordic_0 -- sin and cos generator
  PORT (
    s_axis_phase_tvalid : IN STD_LOGIC;
    s_axis_phase_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axis_dout_tvalid : OUT STD_LOGIC;
    m_axis_dout_tdata : OUT STD_LOGIC_VECTOR(63 DOWNTO 0)
  );
END COMPONENT;

COMPONENT floating_point_7 -- fixed to float converter
  PORT (
    aclk : IN STD_LOGIC;
    s_axis_a_tvalid : IN STD_LOGIC;
    s_axis_a_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axis_result_tvalid : OUT STD_LOGIC;
    m_axis_result_tdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT floating_point_6 -- float to fixed converter
  PORT (
    --aclk : IN STD_LOGIC;
    s_axis_a_tvalid : IN STD_LOGIC;
    s_axis_a_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axis_result_tvalid : OUT STD_LOGIC;
    m_axis_result_tdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_0 -- 1 bit 2 clock register
  PORT (
    D : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_13 -- 1 bit 4 clocks register
  PORT (
    D : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_4 -- 32 bits 1 clock register
  PORT (
    D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_14 -- 4 bits 5 clocks register
  PORT (
    D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_15 -- 32 bits 2 clock register
  PORT (
    D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_2 -- 32 bits 3 clocks register
  PORT (
    D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

COMPONENT c_shift_ram_12 -- 4 bits 2 clocks register
  PORT (
    D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    CLK : IN STD_LOGIC;
    Q : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
  );
END COMPONENT;

COMPONENT floating_point_2 -- sum
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

COMPONENT floating_point_4 -- multiplier
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

COMPONENT floating_point_8 -- comparer
  PORT (
    s_axis_a_tvalid : IN STD_LOGIC;
    s_axis_a_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    s_axis_b_tvalid : IN STD_LOGIC;
    s_axis_b_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axis_result_tvalid : OUT STD_LOGIC;
    m_axis_result_tdata : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END COMPONENT;

begin

cordic : cordic_0
  PORT MAP (
    s_axis_phase_tvalid => valid,
    s_axis_phase_tdata => phaseFixed,
    m_axis_dout_tvalid => cordicValid,
    m_axis_dout_tdata => resultFixed
  );

fixedToFloat : floating_point_7
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => valid,
    s_axis_a_tdata => sinFixed,
    m_axis_result_tvalid => FixToFltValid,
    m_axis_result_tdata => sinFloat
  );
  

floatToFixed : floating_point_6
  PORT MAP (
    --aclk => clk,
    s_axis_a_tvalid => valid,
    s_axis_a_tdata => phaseFloat,
    m_axis_result_tvalid => FltToFixValid,
    m_axis_result_tdata => phaseFixed
  );
  
tWr_enRegister : c_shift_ram_0
  PORT MAP (
    D => wr_en,
    CLK => CLK,
    Q => tRamW_enA
  );

hWr_enRegister : c_shift_ram_13
  PORT MAP (
    D => wr_en,
    CLK => CLK,
    Q => wr_enBH
  );

integrationRegister : c_shift_ram_4
  PORT MAP (
    D => integrationPass,
    CLK => clk,
    Q => integrationPassReg
  );
  
amplitudeRegister1 : c_shift_ram_4
  PORT MAP (
    D => amplitude,
    CLK => clk,
    Q => amplitudeReg1
  );
  
amplitudeRegister2 : c_shift_ram_15
  PORT MAP (
    D => amplitude,
    CLK => clk,
    Q => amplitudeReg2
  );

offSetRegister1 : c_shift_ram_4
  PORT MAP (
    D => offSet,
    CLK => clk,
    Q => offSetReg1
  );
 
offSetRegister2 : c_shift_ram_2
  PORT MAP (
    D => offSet,
    CLK => CLK,
    Q => offSetReg2
  );
  
addrBTregister : c_shift_ram_14
  PORT MAP (
    D => tRamAddressB,
    CLK => CLK,
    Q => tRamAddressBreg
  );
  
addrATregister : c_shift_ram_12
  PORT MAP (
    D => tRamAddressB,
    CLK => CLK,
    Q => tRamAddressA
  );
  
sumPhase : floating_point_2
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => valid,
    s_axis_a_tdata => integrationPass,
    s_axis_b_tvalid => valid,
    s_axis_b_tdata => phaseOld,
    m_axis_result_tvalid => sumValid,
    m_axis_result_tdata => sum1
  );
  
  
sumNegativePi : floating_point_2
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => valid,
    s_axis_a_tdata => integrationPass,
    s_axis_b_tvalid => valid,
    s_axis_b_tdata => negativePi,
    m_axis_result_tvalid => sum2Valid,
    m_axis_result_tdata => sum2
  );
comparer : floating_point_8
  PORT MAP (
    s_axis_a_tvalid => valid,
    s_axis_a_tdata => sum1,
    s_axis_b_tvalid => valid,
    s_axis_b_tdata => pi,
    m_axis_result_tvalid => comparerValid,
    m_axis_result_tdata => comparation
  );
  
multiplier : floating_point_4
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => valid,
    s_axis_a_tdata => sinFloat,
    s_axis_b_tvalid => valid,
    s_axis_b_tdata => amplitudeReg2,
    m_axis_result_tvalid => multValid,
    m_axis_result_tdata => mult
  );
  
sumOffSet : floating_point_2
  PORT MAP (
    aclk => clk,
    s_axis_a_tvalid => valid,
    s_axis_a_tdata => mult,
    s_axis_b_tvalid => valid,
    s_axis_b_tdata => offSetReg2,
    m_axis_result_tvalid => sumOffSetValid,
    m_axis_result_tdata => ramInputBH
  );
 
tRAM : blk_mem_gen_4
  PORT MAP (
    clka => clk,
    wea => tRamW_enA,
    addra => tRamAddressA,
    dina => tRamInputA,
    clkb => clk,
    addrb => tRamAddressB,
    doutb => tRamOutputB
  );
  
  
process(clk)
    begin
          if clk'event and clk='1' then
                  case state is
                  
                      when hold =>
                            onSinGen <= '0';
                            doneSinGen <= '0';
                            tRamAddressB <= (others =>'0');
                            if cmd = '1' and (not (sourcesNumber = 0))  then
                                state <= generating;
                                onSinGen <= '1';
                                wr_en <= "1";
                            end if;
                      
                      when generating =>
                            tRamAddressB <= std_logic_vector(unsigned(tRamAddressB)+1);
                            if unsigned(tRamAddressB) = sourcesNumber - 1 then
                                state <= finishing;
                                tRamAddressB <= (others =>'0');
                                wr_en <= "0";
                            end if;
                            
                      when finishing =>
                            counterFinish <= std_logic_vector(unsigned(counterFinish)+1);
                            if counterFinish = "100" then
                                counterFinish <= "000";
                                state <= hold;
                                --onSinGen <= '0';
                                doneSinGen <= '1';
                            end if;
                      end case;
                  end if;
              end process;
                               

-- signals from tRamOutput
integrationPass <= tRamOutputB(127 downto 96) when state = generating or state = finishing 
                   else (others => '0');
phaseOld <= tRamOutputB(95 downto 64)when state = generating or state = finishing 
            else (others => '0');
amplitude <= tRamOutputB(63 downto 32) when state = generating or state = finishing 
            else (others => '0');
offSet <= tRamOutputB(31 downto 0) when state = generating or state = finishing 
          else (others => '0');

-- resultFixed(31 downto 0) is the cosine
sinFixed <= resultFixed(63 downto 32);

-- tRAM write
tRamInputA <= integrationPassReg & phaseFloat & amplitudeReg1 & offSetReg1;

-- hRAM addressing
ramAddressBH <= std_logic_vector(unsigned(tRamAddressBreg)+nodesNumber);

-- phase controll
phaseFloat <= sum1 when comparation(0) = '1' else sum2;

end Behavioral;
