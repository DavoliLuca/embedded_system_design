library ieee;
use ieee.std_logic_1164.all;

entity decoder_5_6 is
	port ( i1, i2, i3, i4, ck: in std_logic;
           msg: in std_logic_vector(7 downto 0);
	       o1, o2, o3, o4, mv, pr, picked, err: out std_logic);
end entity decoder_5_6;

architecture arc_decoder_5_6 of decoder_5_6 is
    signal pr_state: std_logic:='0';
    signal mv_state: std_logic:='0';
    signal idle_state: std_logic:='0';
    signal combined_state: std_logic_vector(2 downto 0) := "000";
    signal previous_i4: std_logic;
begin
    process(combined_state, i1, i2, i3, i4) is
        variable combined_input: std_logic_vector(3 downto 0);
    begin
        o1 <= '0';
        o2 <= '0';
        o3 <= '0';
        o4 <= '0';
        pr <= combined_state(0);
        mv <= combined_state(2);
        picked <= '0';
        err <= '0';
        combined_input := i4 & i3 & i2 & i1;
        if combined_state(2) then -- Con
            mv <= '0';
            case combined_input is
                when "0001" =>
                    o1 <= '1';
                when "0010" =>
                    o2 <= '1';
                when "0100" =>
                    o3 <= '1';
                when "1000" =>
                    o4 <= '1';
                when "0000" =>
                    mv <= '1';
                when others =>
                    err <= '1';
            end case;
        elsif combined_state(0) = '1' and i4 = '0' and previous_i4 = '1' then -- Picked condition
            picked <= '1';
        elsif combined_state(1) then -- Idle should be interrupted when vial at first station
            if (combined_input = "0001") then
                o1 <= '1';
            end if;
        end if;
    end process;

    process(msg) is  
    begin
        combined_state <= "000";
        if (msg = "10000000") then -- Process
            combined_state <= "001";
        elsif (msg = "00000000" or msg = "00001001") then -- Idle
            combined_state <= "010";
        elsif (msg = "00010000") then -- Move
            combined_state <= "100";
        end if;
    end process;

    process(ck) is
    begin
        if (ck'event and ck = '1') then
            previous_i4 <= i4;
        end if;
    end process;


end arc_decoder_5_6;