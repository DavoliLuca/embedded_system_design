library ieee;
use ieee.std_logic_1164.all;

entity write_uart_sm is
	port ( state_vector: in std_logic_vector(5 downto 0);
           ext_error: in std_logic;
           clk: in std_logic;
	       data_write: out std_logic_vector(5 downto 0); 
           data_write_bool: out std_logic);
end entity write_uart_sm;

architecture arc_write_uart_sm of write_uart_sm is
    signal state_changed: std_logic;
    signal current_state: std_logic_vector(5 downto 0);
begin
    process(clk) is
    begin
        if (clk = '1' and clk'event) then
            data_write <= state_vector;
            if (state_vector /= current_state and ext_error = '0') then
                state_changed <= '1';
                current_state <= state_vector;
                data_write_bool <= '1';
            else
                data_write_bool <= '0';
            end if;
        end if;
    end process;
end arc_write_uart_sm;