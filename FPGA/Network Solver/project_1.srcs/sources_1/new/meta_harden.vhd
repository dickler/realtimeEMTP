--<insert: c:\HW\releasedULD\headers\meta_harden.head>
-- -----------------------------------------------------------------------------
--
-- module:    debouncer
-- project:   wave_gen
-- company:   Xilinx, Inc.
-- author:    WK, AW
-- 
-- comment:
--   Simple switch debouncer. Filters out any transition that lasts less than
--   FILTER clocks long
-- 
-- known issues:
-- status           id     found     description                      by fixed date  by    comment
-- 
-- version history:
--   version    date    author     description
--    11.1-001 20 APR 2009 WK       First version for 11.1          
-- 
-- ---------------------------------------------------------------------------
-- 
-- disclaimer:
--   Disclaimer: LIMITED WARRANTY AND DISCLAMER. These designs  are
--   provided to you as is . Xilinx and its licensors make, and  you
--   receive no warranties or conditions, express,  implied,
--   statutory or otherwise, and Xilinx specifically disclaims  any
--   implied warranties of merchantability, non-infringement,  or
--   fitness for a particular purpose. Xilinx does not warrant  that
--   the functions contained in these designs will meet  your
--   requirements, or that the operation of these designs will  be
--   uninterrupted or error free, or that defects in the  Designs
--   will be corrected. Furthermore, Xilinx does not warrant  or
--   make any representations regarding use or the results of  the
--   use of the designs in terms of correctness,  accuracy,
--   reliability, or  otherwise.
--   
-- LIMITATION OF LIABILITY. In no event will Xilinx or  its
--   licensors be liable for any loss of data, lost profits,  cost
--   or procurement of substitute goods or services, or for  any
--   special, incidental, consequential, or indirect  damages
--   arising from the use or operation of the designs  or
--   accompanying documentation, however caused and on any  theory
--   of liability. This limitation will apply even if  Xilinx
--   has been advised of the possibility of such damage.  This
--   limitation shall apply not-withstanding the failure of  the
--   essential purpose of any limited remedies  herein.
--   
-- Copyright © 2002, 2008, 2009 Xilinx,  Inc.
--   All rights reserved
-- 
-- -----------------------------------------------------------------------------
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity meta_harden is
    Port ( clk_dst          : in  std_logic;
            rst_dst         : in  std_logic;
           signal_src       : in  std_logic;
           signal_dst       : out std_logic);
end meta_harden;


architecture Behavioral of meta_harden is
       signal signal_meta : std_logic := 'U';    -- this signal is more likely to be meta-stable
    begin

       -- behaviorally coded meta-hardener
       getHard: process (clk_dst)             
          begin
             if rising_edge(clk_dst) then        -- detect synchronous events
                if (rst_dst = '1') then          -- if reset is asserted
                   signal_meta <= '0';           -- clear the output of the first flip-flop
                   signal_dst  <= '0';           -- clear the output of the second and final flip-flop
                else                             -- do non-reset activities
                   signal_meta <= signal_src;    -- capture the arriving signal - higher probability of being meta-stable
                   signal_dst  <= signal_meta;   -- resample the potentially meta-stable signal, lowering the probability of meta-stability
                end if;                          -- end of reset/non-reset activities
             end if;                             -- end of synchronous event check
          end process getHard;

    end Behavioral;

