library ieee;
use ieee.std_logic_1164.all;

entity decoder_5_6 is
	port ( i1, i2, i3, i4, pr: in std_logic;
	       o1, o2, o3, o4, mv, pr_st, err: out std_logic);
end entity decoder_5_6;

architecture arc_decoder_5_6 of decoder_5_6 is

begin
    process(i1, i2, i3, i4, pr) is
        variable combined_input: std_logic_vector(4 downto 0);
    begin
        combined_input := pr & i4 & i3 & i2 & i1;
        o1 <= '0';
        o2 <= '0';
        o3 <= '0';
        o4 <= '0';
        mv <= '0';
        pr_st <= '0';
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
                mv <= '1';
            when "10000" =>
                pr_st <= '1';
            when others =>
                err <= '1';
        end case;
    end process;
end arc_decoder_5_6;