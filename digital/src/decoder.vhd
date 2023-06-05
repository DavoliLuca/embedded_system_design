library ieee;
use ieee.std_logic_1164.all;

entity decoder_5_6 is
	port ( i1, i2, i3, i4: in std_logic;
           msg: in std_logic_vector(7 downto 0);
	       o1, o2, o3, o4, mv, pr, err: out std_logic);
end entity decoder_5_6;

architecture arc_decoder_5_6 of decoder_5_6 is
    signal idle: std_logic;
begin
    process(msg) is
    begin
        pr <= '0';
        idle <= '0';
        if (msg = "01010000") then
            pr <= '1';
        elsif (msg = "00000000") then
            idle <= '1';
        end if;
    end process;

    process(i1, i2, i3, i4, pr, idle) is
        variable combined_input: std_logic_vector(4 downto 0);
    begin
        combined_input := pr & i4 & i3 & i2 & i1;
        o1 <= '0';
        o2 <= '0';
        o3 <= '0';
        o4 <= '0';
        mv <= '0';
        pr <= '0';
        err <= '0';
        case combined_input is
            when "00001" =>
                o1 <= '1';
            when "00010" =>
                o2 <= '1';
            when "00100" =>
                o3 <= '1';
            when "01000" =>
                o4 <= '1';
            when "00000" =>
                if (idle = '0') then
                    mv <= '1';
                else
                    mv <= '0';
                end if;
            when "10000" | "10010" | "10100" | "11000" =>
                pr <= '1';
            when others =>
                err <= '1';
        end case;
    end process;
end arc_decoder_5_6;