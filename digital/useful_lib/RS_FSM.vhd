library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity rx_fsm is
        generic(TMAX    : natural := 5000;      -- Max time interval (ms)
                SYS_FREQ: natural := 100;       -- System clock frequency (Hz)
				DELAY	: natural := 2000		-- Actual delay (ms)
		);
	port (	rx_data		: in  std_logic_vector(7 downto 0);
			rx_ready	: in  std_logic;
			n_disp_en	: out std_logic;
			set_cm		: out std_logic;
			set_ud		: out std_logic;
			state_int	: out std_logic_vector(2 downto 0);
			clk			: in  std_logic;
			reset   	: in  std_logic
          );
end rx_fsm;

architecture Behavioral of rx_fsm is

-- FSM signals --
type state is (trig_timeout, wait_next_char, decode, store_ud, store_cm, alive, time_out_error);
signal present_state, next_state: state;
signal turn_on_display: std_logic;
signal turn_off_display: std_logic;

-- TIMER signals --
type timer_state is (t_stop, t_run);
signal t_state: timer_state;

signal timer_cnt: integer range 0 to (TMAX * SYS_FREQ) / 1000;
signal t_out_val: integer range 0 to (TMAX * SYS_FREQ) / 1000;

signal st_timer: std_logic;
signal t_out: std_logic; 

begin

--****************************** FSM ****************************************
------------ Combinatorial process for next state and output functions ------
--
comb:process(rx_ready, present_state, t_out)
begin
			-- Assign a default value to the FSM outputs and next_state
			-- This assures combinatorial synthesis
								
	st_timer	<= '0';  -- Disable timer trigger
	t_out_val 	<= (DELAY * SYS_FREQ)/1000; -- RX time out (ms);
	turn_on_display	<= '0';
	turn_off_display<= '0';
	set_cm		<= '0';	-- Don't store any data in to the cm_register
	set_ud		<= '0';	-- Don't store any data in to the ud_register
	
	next_state <= present_state;
    
    case present_state is
	 
        when trig_timeout =>
			state_int 	<= "000";
			st_timer 	<= '1'; -- Start timeout timer
			next_state	<= wait_next_char;

        when wait_next_char =>
			state_int 	<= "001";
           	if (rx_ready = '1') then
				turn_on_display <= '1';
				next_state 	<= decode;
			elsif(t_out = '1') then		-- Check timeout timer
				turn_off_display <= '1';
				next_state <= trig_timeout;
			end if;
            
        when decode =>
			state_int 	<= "010";
			if 		(rx_data = X"64") then
					next_state <= alive;
						
			elsif	(rx_data(7) = '0') then
					next_state <= store_ud;
						
			elsif	(rx_data(7) = '1') then
					next_state <= store_cm;
						
			end if;

        when store_ud =>
				state_int 	<= "011";
                set_ud 		<= '1';
                next_state 	<= trig_timeout;

        when store_cm =>
				state_int 	<= "100";
                set_cm 		<= '1';
                next_state 	<= trig_timeout;
					 
        when alive =>
				state_int 	<= "111";
                next_state 	<= trig_timeout;
					 
        when others =>
				next_state	<= trig_timeout;
    end case;    
end process;
--
---------------- Register process to store FSM current state and handle display ----
--
reg:process(clk)
begin
    if(clk'event and clk='1') then
        if (reset = '1') then
			present_state 		<= trig_timeout;
			n_disp_en			<= '0';				-- Enable display (Serial Error!)
        else
			present_state 		<= next_state;		-- Store next state

		if (turn_on_display = '1') then
				n_disp_en	<= '0';					-- Enable display (Serial alive, no serial error)
		elsif(turn_off_display = '1') then
				n_disp_en	<= '1';					-- Disable display (Serial error)
		end if;
        end if;
     end if;
end process;
--*************************** END  FSM *************************************

--***************************** TIMER ***************************************
-- This process implements a timer useful to create time programmable events. 
-- The time counter is started when the signal "st_timer" become active
-- (just for one clock pulse), elapsed a time interval equal to "t_out_value" 
-- the signal "t_out" become active and remain active up to the next timer 
-- start (st_time).
--
-- st_time	: When activated ('1') the timer is started (stert event);
-- t_out_val	: Interval time duration in ms;
-- t_out	: Become active "t_out_val" milliseconds after the start event
--
timer: process(clk)
begin
	if(clk'event and clk='1') then
		if (reset = '1') then
			timer_cnt <= 0;
			t_out <= '0';
		else		  
        		if(st_timer = '1') then		-- Load counter timer_cnt with timer value interval t_out_val							
					timer_cnt <= t_out_val;
					t_out <= '0';			-- Clear timer output signal t_out

				elsif (timer_cnt > 0) then 		-- Count down
					timer_cnt <= timer_cnt - 1;
					t_out <= '0';
				else
					t_out <= '1';				-- Set timer output signal t_out
				end if;
        	end if;
	end if;
end process;

--*************************** END TIMER ************************************
end Behavioral;