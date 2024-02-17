
function Game_HandlePause ( )

	select ( g_iPauseState )
		case 1:
			SetSpriteDepth ( g_iOverlay, 0 )

			iAlpha = GetSpriteColorAlpha ( g_iOverlay )

			if ( iAlpha < 200 - 16 )
				iAlpha = iAlpha + 16
				SetSpriteColor ( g_iOverlay, 255, 255, 255, iAlpha )
			else
				g_iPauseState = 2
			endif
		endcase

		case 2:
			SetSpriteDepth ( g_iDialog, 0 )

			iAlpha = GetSpriteColorAlpha ( g_iDialog )

			if ( iAlpha < 255 - 16 )
				iAlpha = iAlpha + 16
				SetSpriteColor ( g_iDialog, 255, 255, 255, iAlpha )
			else
				SetSpriteColor ( g_iDialog, 255, 255, 255, 255 )
				g_iPauseState = 3
			endif
		endcase

		case 3:
			if ( g_iTap = 1 )
				if ( g_iTapX < 250 )
					// yes
					g_iPauseState = 4
				else
					// no
					g_iPauseState = 5
				endif
			endif
		endcase

		case 4:
			iAlpha = GetSpriteColorAlpha ( g_iDialog )

			// fade out dialog
			if ( iAlpha > 16 )
				iAlpha = iAlpha - 16
				SetSpriteColor ( g_iDialog, 255, 255, 255, iAlpha )
			else
				SetSpriteColor ( g_iDialog,  255, 255, 255, 0 )
				SetSpriteColor ( g_iOverlay, 255, 255, 255, 0 )

				g_iGameOver   = 1
				g_iPauseState = 0
			endif
		endcase

		case 5:
			iAlpha = GetSpriteColorAlpha ( g_iDialog )

			// fade out dialog
			if ( iAlpha > 16 )
				iAlpha = iAlpha - 16
				SetSpriteColor ( g_iDialog, 255, 255, 255, iAlpha )
			else
				SetSpriteColor ( g_iDialog, 255, 255, 255, 0 )
				g_iPauseState = 6
			endif
		endcase

		case 6:
			iAlpha = GetSpriteColorAlpha ( g_iOverlay )

			if ( iAlpha > 16 )
				iAlpha = iAlpha - 16
				SetSpriteColor ( g_iOverlay, 255, 255, 255, iAlpha )
			else
				SetSpriteColor ( g_iOverlay, 255, 255, 255, 0 )
				g_iPauseState = 0
				g_iGameState = 1
				g_iTap = 0
			endif
		endcase
	endselect

endfunction
