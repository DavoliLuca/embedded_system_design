library ieee;
use ieee.std_logic_1164.all;

entity tof_mimic is
	port ( i1, i2, i3, i4: in std_logic;
           mv_msg: in std_logic_vector(7 downto 0);
	       tof1, tof2, tof3, tof4: out std_logic);
end entity tof_mimic;

architecture arc_tof_mimic of tof_mimic is
    signal mv_counter: std_logic;
begin
    comb: process(i1, i2, i3, i4)
    begin
        tof1 <= i1;
        tof2 <= i2;
        tof3 <= i3;
        tof4 <= i4;
        if (mv_counter) then
            tof1 <= '0';
            tof2 <= '0';
            tof3 <= '0';
            tof4 <= '0';
        end if;
    end process;

    upd: process(mv_msg, i1, i2, i3, i4)
    begin
        if (mv_msg = "01001000" and mv_msg'event) then
            mv_counter <= '1';
        elsif (i1'event and i1 = '0') then
            mv_counter <= '0';
        elsif (i2'event and i2 = '0') then
            mv_counter <= '0';
        elsif (i3'event and i3 = '0') then
            mv_counter <= '0';
        elsif (i4'event and i4 = '0') then
            mv_counter <= '0';
        end if;
    end process;
end arc_tof_mimic;
