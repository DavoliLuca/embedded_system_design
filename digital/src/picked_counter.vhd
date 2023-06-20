library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
-- per dare un senso agli operatori +1 e -1:
use ieee.std_logic_unsigned.all; 


entity Counter is

	port 
	(
		Q : out  std_logic_vector(7 downto 0);
		Inc: in std_logic;
		Clk : in std_logic
	);

end Counter;

architecture behav of Counter is
signal currVal  :  std_logic_vector(7 downto 0):="00000000";
signal nextVal :  std_logic_vector(7 downto 0);
signal previous_inc_val : std_logic:='0';
begin

	process(clk)
	begin
		if (clk='1' and clk'event) then
			if (Inc ='1' and Inc /= previous_inc_val) then	
				currVal <= (currVal + 1);
			else
				currVal <= currVal;
			end if;
			previous_inc_val <= Inc;
		end if;
	end process;
					  
	Q <= currVal;
			
end behav;